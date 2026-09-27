//
// Created by kohy on 5/12/25.
//

#ifndef EI_FRAME_H
#define EI_FRAME_H
#include "ei_widgetclass.h"
#include "ei_widget_configure.h"
#include "ei_implementation.h"

typedef struct ei_impl_frame_t ei_impl_frame_t;
struct ei_impl_frame_t
{
	ei_impl_widget_t     	widget;
	ei_relief_t		relief;
	ei_string_t		text;
	ei_font_t		text_font;
	ei_color_t		text_color;
	ei_anchor_t		text_anchor;
	ei_surface_t		img;
	ei_rect_ptr_t		img_rect;
	ei_anchor_t		img_anchor;
};
/**
 * \brief	Initializes a frame
 *
 * @param 	wclass		Pointer to an ei_widgetclass_t structure that is be filled with
 *               		the frame class information.
 */
void ei_init_frameclass(ei_widgetclass_t* wclass);

static inline void ei_frame_set_relief	(ei_widget_t frame,  ei_relief_t* relief)	{ ei_frame_configure(frame, NULL, NULL, NULL, relief, NULL, NULL, NULL, NULL, NULL, NULL, NULL); }

#endif //EI_FRAME_H
