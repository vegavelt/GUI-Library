//
// Created by kohy on 5/14/25.
//

#ifndef EI_WIDGET_FUNC_H
#define EI_WIDGET_FUNC_H
#include "ei_types.h"

/**
 * \brief	Removes a widget from its parent's sibling list.
 *
 * @param	widget    The widget to be removed from its sibling list.
 *
 */
void ei_remove_widget_from_siblings(ei_widget_t widget);

/**
 * \brief	Adds a widget as a child to a parent widget.
 *
 * @param	parent    The parent widget to which the child will be added.
 *
 * @param	child     The child widget to be added to the parent's list of children.
 *
 */
void ei_add_child(ei_widget_t parent, ei_widget_t child);

/**
 * \brief	Returns the last sibling of the given widget.
 *
 * @param	widget    The widget whose last sibling is to be retrieved.
 *
 * @return    The last sibling widget of the specified widget, or `NULL` if the widget has no last sibling.
 *
 */
ei_widget_t ei_widget_get_last_sibling(ei_widget_t widget);

/**
 * \brief	Converts a pick identifier (pickid) to a corresponding color.
 *
 * @param	pickid    A 32-bit integer representing a pick identifier.
 *
 * @return    An `ei_color_t` structure representing the color corresponding to the given `pickid`.
 */
ei_color_t ei_pickid_to_pickcolor(uint32_t pickid);

/**
 * \brief	Destroys all widget classes in the widget class registry.
 *
 */
void ei_widgetclass_destroy();

/**
 * \brief	Finds a widget based on its pick color.
 *
 *
 * \param	widget        The widget to start the search from. It can be the root widget or any widget in the widget tree.
 *
 * \param	pick_color    The color used to identify the widget. Each widget is assigned a unique pick color for identification during picking operations.
 *
 * \return	ei_widget_t   The widget that matches the specified pick color, or NULL if no matching widget is found.
 */
ei_widget_t ei_find_widget_from_pickcolor(ei_widget_t, ei_color_t pick_color);

/**
 * \brief  Gets a pick color based on a pixel address.
 *
 * \param  pixel_adr   Pointer to the pixel address whose color will be assigned as the pick color.
 *
 * \return ei_color_t The color (in the form of an `ei_color_t` structure) of the pixel at the given address.
 *                    Returns a color with values (0, 0, 0, 0) if the provided pixel address is `NULL`.
 */
ei_color_t ei_assign_pick_color(uint32_t* pixel_adr);

/**
 * \brief  Destroys the parent widget of the specified widget.
 *
 * \param  widget   The widget whose parent will be destroyed.
 *
 */
void ei_destroy_parent(ei_widget_t widget);

/**
 * \brief  Checks if the widget is of the "toplevel" widget class.
 *
 * \param  widget  The widget to check.
 *
 * \return `true` if the widget is of the "toplevel" class, `false` otherwise.
 *
 */
bool ei_is_toplevelclass(ei_widget_t widget);

/**
 * \brief  Checks if the widget is of the "frame" widget class.
 *
 * \param  widget  The widget to check.
 *
 * \return `true` if the widget is of the "frame" class, `false` otherwise.
 *
 */
bool ei_is_frameclass(ei_widget_t widget);

/**
 * \brief  Checks if the widget is of the "toplevel" widget class.
 *
 * \param  widget  The widget to check.
 *
 * \return `true` if the widget is of the "button" class, `false` otherwise.
 *
 */
bool ei_is_buttonclass(ei_widget_t widget);

/**
 * \brief  Checks if the widget's class is registered in the widget class registry.
 *
 * \param  widget  The widget whose class is being checked.
 *
 * \return `true` if the widget's class is registered, `false` otherwise.
 *
 */
bool ei_is_widgetclass_in_register(ei_widget_t widget);


/**
 * \brief  Retrieves the topmost toplevel window widget among the children of a given widget.
 *
 * \param  widget  The widget whose children are being searched.
 *
 * \return  The topmost "toplevel" widget if found, `NULL` if no such widget is found.
 */
ei_widget_t ei_get_topmost_window(ei_widget_t widget);


#endif //EI_WIDGET_FUNC_H
