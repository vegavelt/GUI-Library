//
// Created by vegavelt on 5/7/25.
//


#include "ei_button.h"
#include "ei_types.h"
#include "ei_event.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "hw_interface.h"

int main()
{
    ei_surface_t		main_window		= NULL;
    ei_size_t			main_window_size	= ei_size(640, 480);
    ei_event_t			event;
    ei_color_t          color               = {0xCC, 0xF0, 0x0F, 0x00};
    ei_point_t          top_left            = {100, 100};
    ei_size_t			size                = {400, 300};
    ei_rect_t			clipper             = {top_left, size};
    // Init access to hardware.
    hw_init();

    // Create the main window.
    main_window = hw_create_window(main_window_size, false);

    // Lock the surface for drawing,
    hw_surface_lock(main_window);
    ei_point_t p1 = {100, 100};
    ei_point_t p2 = {45, 89};
    ei_point_t p3 = {18, 199};
    ei_point_t p4 = {4, 300};
    ei_point_t p5 = {10, 410};

    ei_point_t point_array[] = {p1, p2, p3, p4, p5};
    int point_array_size = 5;




    ei_point_t center = {300,300};
    int radius = 300;
    int start_angle = 0;
    int end_angle = 0;
    int circle_size=0;
    ei_point_t* circle = arc(center, radius, start_angle, end_angle, &circle_size);

    hw_surface_unlock(main_window);
    hw_surface_update_rects(main_window, NULL);

    //Function to test
    ei_draw_polygon(main_window, circle, circle_size, (ei_color_t){0, 0, 255, 255}, NULL);
    //



    // Wait for a key press.
    event.type = ei_ev_none;
    while ((event.type != ei_ev_close) && (event.type != ei_ev_keydown))
        hw_event_wait_next(&event);

    // Free window
    hw_surface_free(main_window);

    // Free hardware resources.
    hw_quit();

    return 0;
}




