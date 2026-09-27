//
// Created by guyal on 5/16/25.
//

#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget_configure.h"
#include "ei_placer.h"


int main(void)
{
  	//1er widget fils de la racine
	ei_widget_t	frame;
	/* Create the application and change the color of the background. */
	ei_app_create((ei_size_t){600, 600}, false);
	ei_frame_set_bg_color(ei_app_root_widget(), (ei_color_t){0x52, 0x7f, 0xb4, 0xff});
	/* Create, configure and place the frame on screen. */
	frame = ei_widget_create	("frame", ei_app_root_widget(), NULL, NULL);
	ei_frame_configure		(frame, &(ei_size_t){300,200},
						   &(ei_color_t){0x88, 0x88, 0x88, 0xff},
						 &(int){6},
						 &(ei_relief_t){ei_relief_raised}, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	ei_place_xy			(frame, 150, 200);

        //2nd widget fils de la racine
	ei_widget_t	frame2;
	frame2 = ei_widget_create	("frame", ei_app_root_widget(), NULL, NULL);
	ei_frame_configure		(frame2, &(ei_size_t){300,200}, &(ei_color_t){0x78, 0x68, 0x88, 0xff},
						 &(int){6}, &(ei_relief_t){ei_relief_raised}, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	int x=20;
	int y=20;
	int width=50;
	int height = 50;
	float rel_x=0.1f, rel_y=0.7f;
	float rel_width=0.3f, rel_height=0.5f;
	ei_anchor_t ancor = ei_anc_north;
	ei_place(frame2, &ancor, &x, &y, &width, &height, NULL, NULL, NULL, NULL);

        // widget fils de widget frame
	ei_widget_t	frame_child;
	frame_child = ei_widget_create	("frame", frame, NULL, NULL);
	ei_frame_configure		(frame_child, &(ei_size_t){300,200}, &(ei_color_t){0x88, 0x78, 0x88, 0xff},
						 &(int){6}, &(ei_relief_t){ei_relief_raised}, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	int x_child=0;
	int y_child=20;
	int width_child=50;
	int height_child = 50;
	float rel_x_child=0.1f, rel_y_child=0.7f;
	float rel_width_child=0.3f, rel_height_child=0.5f;
	ei_anchor_t ancor_child = ei_anc_northeast;
	ei_place(frame_child, &ancor_child, &x_child, &y_child, &width_child, &height_child, NULL, NULL, NULL, NULL);


	/* Run the application's main loop. */
	ei_app_run();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_app_free();

	return (EXIT_SUCCESS);
}
