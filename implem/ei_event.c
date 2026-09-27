//
// Created by vegavelt on 5/12/25.
//

#include "ei_implementation.h"
#include "ei_event_func.h"

static ei_event_info event_info = {
	.active_widget = NULL,
	.state = ei_none,
	{0, 0},
	NULL
};


bool is_widget_active(void) {
	return event_info.active_widget != NULL;
}

void	ei_event_set_active_widget(ei_widget_t widget)
{
	event_info.active_widget = widget;
}

ei_widget_t ei_event_get_active_widget(void)
{
	return event_info.active_widget;
}

void ei_event_set_state(ei_event_state_t state) {
	event_info.state = state;
}

ei_event_state_t ei_event_get_state(void) {
	return event_info.state;
}

void ei_event_set_mouse_initial_position(ei_point_t mouse_initial_position) {
	event_info.mouse_initial_position = mouse_initial_position;
}

ei_point_t ei_event_get_mouse_initial_position(void) {
	return event_info.mouse_initial_position;
}

void ei_event_set_default_handle_func(ei_default_handle_func_t func)
{
	event_info.handle_func = func;
}

ei_default_handle_func_t	ei_event_get_default_handle_func(void)
{
 	return event_info.handle_func;
}

bool is_left_m_button(ei_event_t event) {
	return event.param.mouse.button == ei_mouse_button_left;
}

bool is_right_m_button(ei_event_t event) {
	return event.param.mouse.button == ei_mouse_button_right;
}

bool mouse_on_parent(ei_widget_t widget, ei_event_t event) {
	if (widget == NULL) return false;
	ei_rect_t rect = widget->parent->screen_location;
	ei_point_t where = event.param.mouse.where;
	return where.x > rect.top_left.x &&
		where.x < rect.top_left.x + rect.size.width &&
		where.y > rect.top_left.y &&
		where.y < rect.top_left.y + rect.size.height;
}


