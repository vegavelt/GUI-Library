/**
 * @file	ei_implementation.h
 *
 * @brief 	Private definitions.
 * 
 */

#ifndef EI_IMPLEMENTATION_H
#define EI_IMPLEMENTATION_H

#include "ei_placer.h"

/**
 * \brief	A structure storing the placement parameters of a widget.
 *		You have to define this structure: no suggestion provided.
 */
typedef struct ei_impl_placer_params_t {
	ei_anchor_t anchor;
	int x;
	int y;
	int width;
	int height;
	float rel_x;
	float rel_y;
	float rel_width;
	float rel_height;
} ei_impl_placer_params_t;


/**
 * \brief	Tells the placer to recompute the geometry of a widget.
 *		The widget must have been previsouly placed by a call to \ref ei_place.
 *		Geometry re-computation is necessary for example when the text label of
 *		a widget has changed, and thus the widget "natural" size has changed.
 *
 * @param	widget		The widget which geometry must be re-computed.
 */
void ei_impl_placer_run(ei_widget_t widget);



/**
 * \brief	Fields common to all types of widget. Every widget classes specializes this base
 *		class by adding its own fields.
 */
typedef struct ei_impl_widget_t {
	ei_widgetclass_t*	wclass;		///< The class of widget of this widget. Avoids the field name "class" which is a keyword in C++.
	uint32_t		pick_id;	///< Id of this widget in the picking offscreen.
	ei_color_t		pick_color;	///< pick_id encoded as a color.
	int		 	border_width;
	ei_color_t		background_color;
	void*			user_data;	///< Pointer provided by the programmer for private use. May be NULL.
	ei_widget_destructor_t	destructor;	///< Pointer to the programmer's function to call before destroying this widget. May be NULL.

	/* Widget Hierachy Management */
	ei_widget_t		parent;		///< Pointer to the parent of this widget.
	ei_widget_t		children_head;	///< Pointer to the first child of this widget.	Children are chained with the "next_sibling" field.
	ei_widget_t		children_tail;	///< Pointer to the last child of this widget.
	ei_widget_t		last_sibling;   ///< Pointer to the last child of this widget's parent widget.
	ei_widget_t		next_sibling;	///< Pointer to the next child of this widget's parent widget.

	/* Geometry Management */
	ei_impl_placer_params_t* placer_params;	///< Pointer to the placer parameters for this widget. If NULL, the widget is not currently managed and thus, is not displayed on the screen.
	ei_size_t		requested_size;	///< See \ref ei_widget_get_requested_size.
	ei_rect_t		screen_location;///< See \ref ei_widget_get_screen_location.
	ei_rect_t*		content_rect;	///< See ei_widget_get_content_rect. By defaults, points to the screen_location.
} ei_impl_widget_t;


/**
 * @brief	Draws the children of a widget.
 * 		The children are draw withing the limits of the clipper and
 * 		the widget's content_rect.
 *
 * @param	widget		The widget which children are drawn.
 * @param	surface		A locked surface where to draw the widget's children.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void		ei_impl_widget_draw_children	(ei_widget_t		widget,
						 ei_surface_t		surface,
						 ei_surface_t		pick_surface,
						 ei_rect_t*		clipper);

/**
 * \brief	Converts the red, green, blue and alpha components of a color into a 32 bits integer
 * 		than can be written directly in the memory returned by \ref hw_surface_get_buffer.
 * 		The surface parameter provides the channel order.
 *
 * @param	surface		The surface where to store this pixel, provides the channels order.
 * @param	color		The color to convert.
 *
 * @return 			The 32 bit integer corresponding to the color. The alpha component
 *				of the color is ignored in the case of surfaces that don't have an
 *				alpha channel.
 */
uint32_t	ei_impl_map_rgba(ei_surface_t surface, ei_color_t color);

ei_surface_t ei_app_picking_surface(void);

ei_rect_t ei_intersection_parent_child(ei_rect_t parent_rect, ei_rect_t child_rect);

/**
 * \brief	Calculates the intersection between the surface qnd the clipper
 *          It also sets the good start point and dimensions of the rectangle which describes the surface to copy.
 *
 * @param	surface  	The surface which in intersection will be calculated with.
 *
 * @param	surface_rect  	Rectangle which will describe the intersection with the clipper.
 *                          Must be sent by the user with the size an origin of the surface.
 *
 * @param	text_rect  	Only used when copying text or images. If not NULL, sets the
 *                      good start point and dimensions of the rectangle which describes the surface to copy.
 *
 * @param	clipper  	The clipper which is used to calculate the intersection with
 */
ei_rect_t intersection_clipper(ei_rect_t surface_rect, ei_rect_t* text_rect, const ei_rect_t* clipper);

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
ei_point_t* rounded_top_corners(const ei_rect_t rectangle, const int radius, int *array_points_size);

/**
 * \brief  Checks if a point is located within the boundaries of a rectangle.
 *
 * \return True if the point is inside or on the boundary of the rectangle;
 *         false if the point is outside the rectangle.
 */
bool is_point_on_rect(ei_rect_t rect, ei_point_t point);

/**
 * \brief  Compares two rectangles to check if they are identical.
 *
 * \param  rect1  The first rectangle to compare, defined by its top-left corner and size (width and height).
 *
 * \param  rect2  The second rectangle to compare, defined by its top-left corner and size (width and height).
 *
 * \return True if both rectangles have the same top-left corner and size; false otherwise.
 */
bool is_same_rect(ei_rect_t rect1, ei_rect_t rect2);

/**
 * \brief  Compares two points to check if they are identical.
 *
 * \param  pt1  The first point to compare, defined by its x and y coordinates.
 *
 * \param  pt2  The second point to compare, defined by its x and y coordinates.
 *
 * \return True if both points have the same x and y coordinates; false otherwise.
 */
bool is_same_point(ei_point_t pt1, ei_point_t pt2);

/**
 * \brief  Compares two sizes to check if they are identical.
 *
 * \param  size1  The first size object to compare, defined by its width and height.
 *
 * \param  size2  The second size object to compare, defined by its width and height.
 *
 * \return True if both sizes have the same width and height; false otherwise.
 */
bool is_same_size(ei_size_t size1, ei_size_t size2);

/**
 * \brief	Recursively places a toplevel widget and its children on the screen.
 *
 * \param widget  The widget (or toplevel widget) to be placed. This widget and its children will be placed on the screen.
 */
void ei_place_toplevel(ei_widget_t widget);

static inline void ei_place_size(ei_widget_t widget, int* width, int* height) { ei_place(widget, NULL, NULL, NULL, width, height, NULL, NULL, NULL, NULL); }

#endif
