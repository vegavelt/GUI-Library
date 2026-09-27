//
// Created by vegavelt on 5/7/25.
// Created by kohy on 5/12/25.
//

#ifndef EI_BUTTON_H
#include "hw_interface.h"
#include "ei_frame.h"

typedef struct ei_impl_button_t ei_impl_button_t;
struct ei_impl_button_t
{
	ei_impl_frame_t     frame;
	int		corner_radius;
	ei_callback_t	callback; // traitant externe
	ei_user_param_t	user_param; // param spécifique à ce btn particulier lors de l'appel du traitement
};


/**
 * \brief	Draws a button in a surface
 *
 * @param	surface  	The surface to draw on.
 *
 * @param	rectangle  	The rectangle to draw.
 *
 * @param	radius 		The radius of the corners
 *
 * @param	border_width 	The width of the border of the button
 *
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 * @param	color		Pointer to the color of the button. If it NULL the default color #646464 will be selected.
 *
 * @param	relief		Choose if the button is sunken, raised or flat.
 */
void draw_button(const ei_surface_t surface, const ei_rect_t rectangle,  int radius, int border_width, ei_rect_t* clipper, const ei_color_t* color, ei_relief_t relief);

/**
 * \brief	Initializes a button
 *
 * @param 	wclass		Pointer to an ei_widgetclass_t structure that is be filled with
 *               		the button class information.
 */
void ei_init_buttonclass(ei_widgetclass_t* wclass);


/**
 * \brief	Calculates the updated coordinates of the text or image based on the specified anchor.
 *
 * @param 	anchor		Reference anchor of the text.
 *
 * @param 	parent_rect	Rectangle with the size and de top left coordinates of the button
 *
 * @param 	x_where		Pointer to the top left x-coordinate of the text. Initialy the user should pass the top left x-coordinate of the button.
 *				The pointed value will be modified based on the selected anchor.
 *
 *
 * @param 	y_where		Pointer to the top left y-coordinate of the text. Initialy the user should pass the top left y-coordinate of the button.
 *				The pointed value will be modified based on the selected anchor.
 *
 * @param 	widget_center	Point with the coordinates of the middle of the widget.
 *
 */
void ei_text_anchor_coordinate(ei_anchor_t anchor, int *x_where, int *y_where, ei_point_t text_center, ei_point_t widget_center);

/**
 * \brief	Creates the close button widget for the title bar of a toplevel widget.
 *
 * @param	widget        The widget that will contain the title bar button.
 *
 */
void ei_create_button_title_bar(ei_widget_t widget);

#endif //EI_BUTTON_H
