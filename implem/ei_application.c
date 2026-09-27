//
// Created by guyal on 5/7/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_application.h"
#include <string.h>
#include "ei_button.h"
#include "ei_frame.h"
#include "ei_types.h"
#include "ei_widgetclass.h"
#include "ei_implementation.h"
#include "ei_toplevel.h"
#include "ei_widget_attributes.h"
#include "ei_widget_func.h"

//variable global root
static ei_widget_t root_widget = NULL;
static ei_surface_t main_window = NULL;
static ei_surface_t off_screen = NULL;

static void ei_draw_widgets(ei_widget_t widget);
static bool ei_handle_event(ei_event_t* event, double* last_update_time);
static void ei_update_root_surface(void);

void ei_app_create(ei_size_t main_window_size, bool fullscreen)
{
    hw_init();

    // Register toplevel
    ei_widgetclass_t* toplevel_class = malloc(sizeof(ei_widgetclass_t));
    ei_init_toplevelclass(toplevel_class);
    ei_widgetclass_register(toplevel_class);

    // Register frame
    ei_widgetclass_t* frame_class = malloc(sizeof(ei_widgetclass_t));
    ei_init_frameclass(frame_class);
    ei_widgetclass_register(frame_class);

    // Register button
    ei_widgetclass_t* button_class = malloc(sizeof(ei_widgetclass_t));
    ei_init_buttonclass(button_class);
    ei_widgetclass_register(button_class);

    // TODO delete this before hand in the project
    // temporary picking offscreen
    // off_screen = hw_create_window(main_window_size, fullscreen);

    // create the main window
    main_window = hw_create_window(main_window_size, fullscreen);

    // create the root widget (frame)
    root_widget = ei_widget_create("frame", NULL, NULL, NULL);

    // set content rect of the root widget
    // must stock this in memory if not when the stack is reused by other functions,
    // it will change the value of this pointer
    root_widget->screen_location = (ei_rect_t){{0, 0}, fullscreen ? hw_surface_get_size(main_window) : main_window_size};
    root_widget->content_rect = malloc(sizeof(ei_rect_t));
    ei_widget_set_content_rect(root_widget, ei_widget_get_screen_location(root_widget));

    // picking offscreen
    off_screen = hw_surface_create(main_window, main_window_size, fullscreen);
}

static bool quit_request = false;

void ei_app_run(void)
{
    ei_place_toplevel(root_widget);
    ei_update_root_surface();
    ei_event_t event;
    double last_update_time_event = hw_now();
    double last_update_time_per_second = hw_now();
    while (!quit_request) {
        hw_event_wait_next(&event);
        bool event_handled = ei_handle_event(&event, &last_update_time_event);
        double time_diff = hw_now() - last_update_time_per_second;
        if (event_handled || time_diff >= 0.7) {
            ei_update_root_surface();
            last_update_time_per_second = hw_now();
        }
    }
}

void ei_app_free(void)
{
    // destroy the widget and its descendents
    ei_widget_destroy(ei_app_root_widget());
    // destroy all the instances of classes created
    ei_widgetclass_destroy();
    // destroy the main_window and off-screen
    hw_surface_free(off_screen);
    hw_surface_free(main_window);
    hw_quit();
}

ei_widget_t ei_app_root_widget(void)
{
    return root_widget;
}

void ei_app_quit_request(void)
{
    quit_request = true;
}

ei_surface_t ei_app_root_surface(void)
{
    return main_window;
}

ei_surface_t ei_app_picking_surface(void) {
    return off_screen;
}

/**
 * \brief	Adds a rectangle to the list of rectangles that must be updated on screen. The real
 *		update on the screen will be done at the right moment in the main loop.
 *
 * @param	rect		The rectangle to add, expressed in the root window coordinates.
 *				A copy is made, so it is safe to release the rectangle on return.
 */
void ei_app_invalidate_rect(const ei_rect_t* rect) {
    // TODO display manager (not yet there!)

}

static void ei_update_root_surface(void) {
    hw_surface_lock(main_window);
    hw_surface_lock(off_screen);
    ei_draw_widgets(ei_app_root_widget());
    hw_surface_unlock(off_screen);
    hw_surface_unlock(main_window);
    hw_surface_update_rects(main_window, NULL);
    // TODO remove this in the end
    //hw_surface_update_rects(off_screen, NULL);
}

static void ei_draw_widgets(ei_widget_t widget) {
    // Draw the root widget
    widget->wclass->drawfunc(widget, ei_app_root_surface(), ei_app_picking_surface(), widget->content_rect);
    // Draw all the descendents of the root widgets
    ei_impl_widget_draw_children(widget, ei_app_root_surface(), ei_app_picking_surface(), widget->content_rect);
}

static bool ei_handle_event(ei_event_t* event, double* last_update_time) {
    ei_widget_t active_widget = ei_event_get_active_widget();
    ei_widget_t widget_picked;
    // If the widget is not NULL
    // then call the handlefunc of this widget (general handle function of a widget)
    bool event_handled = false;

    if (active_widget != NULL) {
        switch (event->type) {
            case ei_ev_mouse_move:
                widget_picked = ei_widget_pick(&event->param.mouse.where);
                // For toplevel as it redraws too much
                double min_frame_time = 0.0625; //16 Hz
                double current_time = hw_now();
                // Skip handling for toplevel moves (drag) that are too frequent
                // Skip when the time difference between two events are less than the threshold
                // No need to set event_handled since we're skipping
                if (!(ei_widgetclass_from_name("toplevel") == active_widget->wclass &&
                    current_time - *last_update_time < min_frame_time)) {
                    // Handle all other active widget events normally
                    event_handled = active_widget->wclass->handlefunc(widget_picked, event);
                    *last_update_time = current_time;
                }
                break;

            case ei_ev_mouse_buttonup:
                widget_picked = ei_widget_pick(&event->param.mouse.where);
                event_handled = active_widget->wclass->handlefunc(widget_picked, event);
                break;

            default:
                break;
        }
    } else {
        switch (event->type) {
            case ei_ev_mouse_buttondown:
                widget_picked = ei_widget_pick(&event->param.mouse.where);
                if (widget_picked != NULL) {
                    // Set the toplevel focused to the topmost when it is clicked and it is not topmost
                    if (is_toplevel(widget_picked) && ei_get_topmost_window(root_widget) == widget_picked) {
                        event_handled = ei_widgetclass_from_name("toplevel")->handlefunc(get_toplevel_parent(widget_picked), event);
                    } else {
                        event_handled = widget_picked->wclass->handlefunc(widget_picked, event);
                    }
                }
                break;
            default:
                break;
        }
    }

    if (!event_handled) {
        ei_default_handle_func_t default_func = ei_event_get_default_handle_func();
        if (default_func != NULL) {
            ei_event_get_default_handle_func()(event);
            if (event->type == ei_ev_keydown || event->type == ei_ev_mouse_buttondown) {
                ei_widget_t topmost = ei_get_topmost_window(root_widget);
                if (topmost != NULL) {
                    ei_place_xy(topmost,
                    topmost->screen_location.top_left.x - topmost->parent->screen_location.top_left.x,
                    topmost->screen_location.top_left.y - topmost->parent->screen_location.top_left.y);
                    event_handled = true;
                }
            }
        }
    }
    return event_handled;
}




