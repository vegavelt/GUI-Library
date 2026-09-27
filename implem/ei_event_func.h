//
// Created by youchen on 5/15/25.
//

#ifndef EI_EVENT_FUNC_H
#define EI_EVENT_FUNC_H

#include "ei_event.h"

typedef enum {
	ei_none = 0,		///< Inactive
	ei_toplevel_drag,	///< Drag, move a toplevel
	ei_toplevel_resize,	///< Resize a toplevel
	ei_on_button,		///< Mouse is on button
	ei_out_button		///< Mouse is out of button
} ei_event_state_t;

typedef struct ei_event_info {
	ei_widget_t active_widget;		///< widget handled
	ei_event_state_t state;			///< State of a widget
	ei_point_t mouse_initial_position;	///< Initial position of the mouse before event handling
	ei_default_handle_func_t handle_func;	///< Default function defined by the programmer
} ei_event_info;

/**
 * \brief		Checks whether there is an active widget :
 *
 * @returns		True if there is an active widget, false otherwise.
 */
bool is_widget_active(void);

/**
 * \brief	Sets the current event state.
 */
void ei_event_set_state(ei_event_state_t state);

/**
 * \brief		Gets the current event state.
 *
 * @return		The current event state stored in `event_info.state`.
 */
ei_event_state_t ei_event_get_state(void);

/**
 * \brief		Sets the initial position of the mouse.
 */
void ei_event_set_mouse_initial_position(ei_point_t mouse_initial_position);

/**
 * \brief	Gets the initial position of the mouse.
 *
 * @return	The initial position of the mouse stored in "event_info.mouse_initial_position".
 */
ei_point_t ei_event_get_mouse_initial_position(void);

/**
 * \brief		Checks whether the mouse right button in pressed.
 *
 * @return		True if the mouse right button in pressed, false otherwise.
 */
bool is_left_m_button(ei_event_t event);

/**
 * \brief		Checks whether the mouse left button in pressed.
 *
 * @return		True if the mouse left button in pressed., false otherwise.
 */
bool is_right_m_button(ei_event_t event);

/**
 * \brief	Checks if the mouse is within the bounds of a widget's parent.
 *
 * @param	widget    The widget whose parent's bounds are being checked.
 * @param	event     The mouse event containing the current mouse position.
 *
 * @return	True if the mouse pointer is within the bounds of the widget's parent, false otherwise.
 *
 */
bool mouse_on_parent(ei_widget_t widget, ei_event_t event);

#endif //EI_EVENT_FUNC_H
