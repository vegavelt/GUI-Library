//
// Created by kohy on 5/12/25.
//

#include "ei_event.h"
#include "ei_widget_attributes.h"
#include "hw_interface.h"
#include <stdbool.h>
#include "ei_button.h"
#include "ei_application.h"
#include "ei_toplevel.h"
#include "ei_widget_func.h"

// Drawing of a button



static ei_widget_t ei_button_alloc();


/**
 * \brief	Frees the allocated memory of button.
 *
 * @param	widget        The widget that will be free.
 *
 */
static void	ei_button_release		(ei_widget_t	widget);


/**
 * \brief	Draws a button widget, including its background, text, and/or image on the given surface.
 *
 * @param	widget         The button widget to be drawn.
 *
 * @param	surface        The surface on which the button should be drawn.
 *
 * @param	pick_surface   The picking offscreen surface.
 *
 * @param	clipper        A clipping rectangle to limit the drawing area.
 *
 */
static void	ei_button_draw	(ei_widget_t		widget,
							 ei_surface_t		surface,
							 ei_surface_t		pick_surface,
							 ei_rect_t*		clipper);


static void	ei_button_setdefaults	(ei_widget_t		widget);


/**
 * \brief			Updates the widget content rect, after modifying the screen location.
 *
 * @param	widget		The widget whose content rect will be updated.
 *
 */
static void	ei_button_geomnotify	(ei_widget_t		widget);


/**
 * \brief			Handles the events of the button
 *
 * @param	widget		The widget whose events will be handled;
 *
 */
static bool	ei_button_handle		(ei_widget_t		widget,
							  struct ei_event_t*	event);
/**
 * \brief	Initializes the widget class for a button.
 *
 * @param	wclass     A pointer to the widget class to be initialized.
 *
 */
void ei_init_buttonclass(ei_widgetclass_t* wclass)
{
	strcpy(wclass->name, "button");
  	wclass->allocfunc = ei_button_alloc;
	wclass->releasefunc = ei_button_release;
	wclass->drawfunc = ei_button_draw;
	wclass->setdefaultsfunc = ei_button_setdefaults;
	wclass->geomnotifyfunc = ei_button_geomnotify;
	wclass->handlefunc = ei_button_handle;
}

/**
 * \brief	Allocates memory for a button.
 *
 */

static ei_widget_t ei_button_alloc()
{
	return malloc(sizeof(ei_impl_button_t));
}

/**
 * \brief				Sets the default attributes for a button widget.
 *
 * @param	widget		Widget whose default attributes will be set.
 *
 */

static void	ei_button_setdefaults	(ei_widget_t		widget)
{
	// initialize common attributes with frame
	ei_impl_button_t* self = (ei_impl_button_t*)widget;
	ei_widgetclass_from_name("frame")->setdefaultsfunc(&(self->frame.widget));
	self->frame.relief = ei_relief_none;
	self->frame.widget.border_width = k_default_button_border_width;

	// Initialize button attributes
	self->corner_radius = k_default_button_corner_radius;
	self->callback = NULL;
	self->user_param = NULL;
}


void	ei_button_configure		(ei_widget_t		widget,
			 ei_size_t*		requested_size,
			 const ei_color_t*	color,
			 int*			border_width,
			 int*			corner_radius,
			 ei_relief_t*		relief,
			 ei_string_t*		text,
			 ei_font_t*		text_font,
			 ei_color_t*		text_color,
			 ei_anchor_t*		text_anchor,
			 ei_surface_t*		img,
			 ei_rect_ptr_t*		img_rect,
			 ei_anchor_t*		img_anchor,
			 ei_callback_t*		callback,
			 ei_user_param_t*	user_param)
{
	// Configure common attributes with frame
	ei_frame_configure(widget, requested_size, color, border_width, relief, text, text_font, text_color, text_anchor, img, img_rect, img_anchor);
	ei_impl_button_t*	self = (ei_impl_button_t*)widget;
	if (corner_radius != NULL) self->corner_radius = *corner_radius;
	if (callback != NULL) self->callback = *callback;
	if (user_param != NULL) self->user_param = *user_param;
}


static void	ei_button_draw	(ei_widget_t		widget,
							 ei_surface_t		surface,
							 ei_surface_t		pick_surface,
							 ei_rect_t*		clipper)
{
	ei_impl_button_t* self = (ei_impl_button_t*)widget;
	int corner_radius = self->corner_radius;
	int border_width = self->frame.widget.border_width;
	ei_rect_t rectangle = self->frame.widget.screen_location;
	ei_color_t pick_color = self->frame.widget.pick_color;
	ei_relief_t relief = self->frame.relief;
	ei_color_t button_color = self->frame.widget.background_color;
	draw_button(pick_surface, rectangle, corner_radius, border_width, clipper, &pick_color, ei_relief_none);
	draw_button(surface, rectangle, corner_radius, border_width, clipper, &button_color, relief);

	// Draws the text if it exists else draw the image if it exists
	if (self->frame.text != NULL && strlen(self->frame.text) != 0) {
		ei_font_t font = self->frame.text_font;

		if (self->frame.text_font == NULL) font = ei_default_font;
		ei_anchor_t text_anchor = self->frame.text_anchor;

		int width = 0, height = 0;
		hw_text_compute_size (self->frame.text, font, &width, &height);

		ei_point_t widget_center = {rectangle.size.width/2, rectangle.size.height/2};
		int x_where = rectangle.top_left.x, y_where = rectangle.top_left.y;
                int text_width = 0, text_height = 0;

                hw_text_compute_size (self->frame.text, font, &text_width, &text_height);

                ei_point_t text_center = {text_width/2,text_height/2};
		ei_text_anchor_coordinate(text_anchor, &x_where, &y_where, text_center, widget_center);
		ei_point_t where = {x_where, y_where};

		if (relief == ei_relief_sunken)
		{
			int text_offset = rectangle.size.width/64;
			where.x += text_offset;
			where.y += text_offset;
		}
		ei_draw_text(surface, &where, self->frame.text, self->frame.text_font, self->frame.text_color, clipper);
	} else if (self->frame.img != NULL) {
		ei_anchor_t image_anchor = self->frame.img_anchor;
		ei_rect_ptr_t image_rect = self->frame.img_rect;

		if (image_rect == NULL) {
			image_rect = malloc(sizeof(ei_rect_t));
			*image_rect = hw_surface_get_rect(self->frame.img);
		}
		ei_surface_t image = self->frame.img;
		int x_where = rectangle.top_left.x, y_where = rectangle.top_left.y;

		ei_point_t widget_center = {rectangle.size.width/2, rectangle.size.height/2};
		ei_point_t image_center = {image_rect->size.width / 2, image_rect->size.height / 2,};
		ei_text_anchor_coordinate(image_anchor, &x_where, &y_where, image_center, widget_center);

		ei_rect_t dst_rect = {{x_where, y_where}, image_rect->size};
		ei_rect_t image_rect_clipper = rectangle;
		image_rect_clipper.top_left = image_rect->top_left;
		*image_rect = intersection_clipper(*image_rect, NULL, &image_rect_clipper);

		ei_copy_surface(surface, &dst_rect, image ,image_rect, true);
	}
}

void ei_text_anchor_coordinate(ei_anchor_t anchor, int *x_where, int *y_where, ei_point_t text_center, ei_point_t widget_center) {
	switch (anchor) {
		case ei_anc_north:
			*x_where += widget_center.x - text_center.x;
			break;

		case ei_anc_west:
			*y_where += widget_center.y - text_center.y;
			break;

		case ei_anc_center:
			*x_where += widget_center.x - text_center.x;
			*y_where += widget_center.y - text_center.y;
			break;

		case ei_anc_southwest:
			*y_where += 2 * (widget_center.y - text_center.y);
			break;

		case ei_anc_south:
			*x_where += widget_center.x - text_center.x;
			*y_where += 2 * (widget_center.y - text_center.y);
			break;

		case ei_anc_east:
			*x_where += 2*(widget_center.x - text_center.x);
			*y_where += widget_center.y - text_center.y;
			break;

		case ei_anc_northeast:
			*x_where += 2*(widget_center.x - text_center.x);
			break;

		case ei_anc_southeast:
			*x_where += 2*(widget_center.x - text_center.x);
			*y_where += 2 * (widget_center.y - text_center.y);
			break;

		case ei_anc_northwest:
			break;

		default:
			*x_where += widget_center.x - text_center.x;
			*y_where += widget_center.y - text_center.y;
			break;

	}

}


static void	ei_button_geomnotify	(ei_widget_t		widget)
{
	ei_widget_set_content_rect(widget, ei_widget_get_screen_location(widget));
}

static bool	ei_button_handle		(ei_widget_t		widget,
						 	 struct ei_event_t*	event) {
	ei_widget_t active_widget = ei_event_get_active_widget();

	// If active widget exists, means that we have chosen a button
	// so we can just use this button instead of widget picked
	// to make sure it is the button we want
	// else just take the widget picked on the surface
	ei_impl_button_t* button = active_widget == NULL ? (ei_impl_button_t*)widget : (ei_impl_button_t*)active_widget;
	if (button == NULL) return false;
	if (!ei_is_widgetclass_in_register((ei_widget_t)button)) return false;

	bool changed = false;
	// Determine if the mouse is over the button
	ei_rect_t* content_rect = button->frame.widget.content_rect;
	bool is_over_button = content_rect->top_left.x <= event->param.mouse.where.x
		&& content_rect->top_left.x + content_rect->size.width >= event->param.mouse.where.x
		&& content_rect->top_left.y <= event->param.mouse.where.y
		&& content_rect->top_left.y + content_rect->size.height >= event->param.mouse.where.y;

	ei_relief_t current_relief = button->frame.relief;
	ei_relief_t new_relief = current_relief;

	// Case 1 : Mouse left clicked, then change to ei_relief_sunken
	// Case 2 : Mouse released, then change back to ei_relief_raised
	// Case 3 : Mouse moved, over the button, then change to ei_relief_sunken
	// Case 4 : Mouse moved, not over the button, then change back to ei_relief_raised
	switch (event->type) {
		case ei_ev_mouse_buttondown:
			switch (event->param.mouse.button) {
				case ei_mouse_button_left:
					new_relief = ei_relief_sunken;
					ei_event_set_active_widget(widget);
					break;
				case ei_mouse_button_right:
					ei_event_set_active_widget(widget);
					break;
				default:
					break;
			}
			break;

		case ei_ev_mouse_buttonup:
			if (active_widget == widget) {
				// Only trigger callback if releasing while over the button
				if (is_over_button && button->callback != NULL) {
					button->callback(widget, event, button->user_param);
					ei_widget_t topmost = ei_get_topmost_window(ei_app_root_widget());
					if (topmost != NULL) {
						ei_place_xy(topmost,
						topmost->screen_location.top_left.x - topmost->parent->screen_location.top_left.x,
						topmost->screen_location.top_left.y - topmost->parent->screen_location.top_left.y);
					}
				}
			}
			new_relief = ei_relief_raised;
			ei_event_set_active_widget(NULL);
			break;

		case ei_ev_mouse_move:
			if (active_widget != NULL && is_over_button) {
				// This button is currently being pressed
				// Firefox behavior: button appears pressed only when mouse is over it
				new_relief = ei_relief_sunken;
			} else {
				new_relief = ei_relief_raised;
			}
			break;

		default:
			new_relief = ei_relief_raised;
			break;
	}
	// Apply visual change
	if (new_relief != current_relief) {
		ei_frame_set_relief(&button->frame.widget, &new_relief);
		changed = true;
	}
	return changed;
}

static void	ei_button_release		(ei_widget_t	widget)
{
	ei_impl_button_t* self = (ei_impl_button_t*)widget;
	ei_widgetclass_from_name("frame")->releasefunc(&self->frame.widget);
}

void ei_create_button_title_bar(ei_widget_t widget) {
	((ei_impl_toplevel_t*)widget)->title_bar.button = ei_widget_create("button", widget, NULL, NULL);
	ei_widget_t button = ((ei_impl_toplevel_t*)widget)->title_bar.button;

	((ei_impl_button_t*)button)->callback = (void*)ei_destroy_parent;
	ei_button_configure(button,
			&(ei_size_t){ei_font_default_size, ei_font_default_size},
			&(ei_color_t){240, 0, 0, 0xFF},
			NULL,
			&(int){ei_font_default_size/2},
			&(ei_relief_t){ei_relief_sunken},
			NULL, NULL, NULL, NULL,
			NULL, NULL, NULL,
			NULL, NULL
			);
}