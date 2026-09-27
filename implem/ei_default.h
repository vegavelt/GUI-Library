//
// Created by kohy on 5/13/25.
//

#ifndef EI_DEFAULT_H
#define EI_DEFAULT_H
#include "ei_types.h"

/**
 * @brief	The default Toplevel requested size.
 */
static const ei_size_t ei_toplevel_default_requested_size   = {320, 240};

/**
 * @brief	The default Toplevel minimum size.
 */
static const ei_size_t ei_toplevel_default_min_size		    = {160, 120};

/**
 * @brief	The default frame requested size.
 */
static const ei_size_t ei_frame_default_requested_size      = {0, 0};

/**
 * @brief	The default frame border width.
 */
static const int ei_frame_default_border_width              = 0;


/**
 * @brief	The default placer border width.
 */
static const int ei_placer_default_width = 30; // à verif

/**
 * @brief	The default placer border height.
 */
static const int ei_placer_default_height = 30;

/**
 * @brief	The default margin.
 */
static const int ei_default_margin = 5;

/**
 * @brief	The default title bar height.
 */
static const int ei_default_title_bar_height = ei_font_default_size + 2*ei_default_margin;

/**
 * @brief	The default title bar title.
 */
static const ei_string_t ei_default_title = "Toplevel";

#endif //EI_DEFAULT_H
