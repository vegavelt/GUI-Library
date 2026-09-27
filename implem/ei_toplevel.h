//
// Created by kohy on 5/12/25.
//

#ifndef EI_TOPLEVEL_H
#define EI_TOPLEVEL_H
#include "ei_implementation.h"

typedef struct ei_impl_title_bar_t {
	ei_rect_t rect;
	ei_widget_t button;	///< Button pointer to
	ei_string_t title;
} ei_impl_title_bar_t;

typedef struct ei_impl_toplevel_t ei_impl_toplevel_t;
struct ei_impl_toplevel_t
{
	ei_impl_widget_t   	widget;
	ei_impl_title_bar_t	title_bar;
	ei_rect_t		resize_zone;
	bool			closable;
	ei_axis_set_t		resizable;
	ei_size_ptr_t		min_size;
};

/**
 * \brief	Initializes a button
 *
 * @param 	wclass		Pointer to an ei_widgetclass_t structure that is be filled with
 *               		the toplevel class information.
 */
void ei_init_toplevelclass(ei_widgetclass_t* wclass);

/**
 * \brief	Calculates the minimum size of a widget.
 *
 * @param	widget    The widget for which the minimum size is calculated.
 *
 */
int ei_calculate_min_size(ei_widget_t widget);

/**
 * \brief	Creates a point array of the coordinates of a rectangle with only the top corners rounded corners
 *
 * @param	rectangle  	The rectangle to draw.
 *
 * @param	radius 		The radius of the corners
 *
 * @param	*array_points_size Pointer that will indicate the size of the final array. It initial pointed value is not important as it is overwritten during
 * 				   the execution of the funciton.
 */
ei_point_t* rounded_top_corners(ei_rect_t rectangle, int radius, int *array_points_size);


/**
 * \brief	Calculates the minimum size required by comparing the given size with the predefined minimum size.
 *
 * @param	min_size    The predefined minimum size.
 *
 * @param	size        The size to compare with the minimum size.
 *
 * @return	A new size, where the width and height are the greater of the given size and the minimum size.
 *
 */
ei_size_t get_min_size(ei_size_t min_size, ei_size_t size);


/**
 * \brief	Destroys the close button associated with a top-level widget.
 *
 * @param	widget    The widget whose close button is being destroyed.
 *
 */
void ei_destroy_close_button(ei_widget_t widget);

/**
 * \brief	Checks whether the given widget is a top-level widget.
 *
 * @param	widget    The widget to check.
 *
 * @return	`true` if the widget is a top-level widget, `false` otherwise.
 *
 */
bool is_toplevel(ei_widget_t widget);


/**
 * \brief	Recursively retrieves the top-level parent widget of a given widget.
 *
 * @param	widget    The widget whose top-level parent is to be retrieved.
 *
 * @return	The top-level parent widget of the given widget, or `NULL` if no top-level parent exists.
 *
 */
ei_widget_t get_toplevel_parent(ei_widget_t widget);

#endif //EI_TOPLEVEL_H
