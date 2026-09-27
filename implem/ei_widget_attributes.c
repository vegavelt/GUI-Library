/**
 * @file	ei_widget_attributes.h
 *
 * @brief 	API for accessing widgets attributes.
 *
 */

#include "ei_implementation.h"

ei_widgetclass_t*	ei_widget_get_class		(ei_widget_t		widget)
{
	return widget->wclass;
}

const ei_color_t*	ei_widget_get_pick_color	(ei_widget_t		widget)
{
	const ei_color_t*	color = &widget->pick_color;
	return color;
}

ei_widget_t 		ei_widget_get_parent		(ei_widget_t		widget)
{
	return widget->parent;
}

ei_widget_t 		ei_widget_get_first_child	(ei_widget_t		widget)
{
	return widget->children_head;
}

ei_widget_t 		ei_widget_get_last_child	(ei_widget_t		widget)
{
	return widget->children_tail;
}

ei_widget_t ei_widget_get_last_sibling(ei_widget_t widget)
{
	return widget->last_sibling;
}

ei_widget_t 		ei_widget_get_next_sibling	(ei_widget_t		widget)
{
	return widget->next_sibling;
}

void*			ei_widget_get_user_data		(ei_widget_t		widget)
{
	return widget->user_data;
}

const ei_size_t*	ei_widget_get_requested_size	(ei_widget_t		widget)
{
	const ei_size_t*	size = &(widget->requested_size);
	return size;
}

void	 		ei_widget_set_requested_size	(ei_widget_t		widget,
							 ei_size_t 		requested_size)
{
	widget->requested_size = requested_size;
}

const ei_rect_t*	ei_widget_get_screen_location	(ei_widget_t		widget)
{
	const ei_rect_t*	rect = &widget->screen_location;
	return rect;
}

const ei_rect_t*	ei_widget_get_content_rect	(ei_widget_t		widget)
{
	return widget->content_rect;
}

void	 		ei_widget_set_content_rect	(ei_widget_t		widget,
							 const ei_rect_t*	content_rect)
{
	if (widget->content_rect == NULL) {
		widget->content_rect = malloc(sizeof(ei_rect_t));
	}
	*widget->content_rect = *content_rect;
}
