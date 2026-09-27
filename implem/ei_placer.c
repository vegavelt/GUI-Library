//
// Created by guyal on 5/7/25.
//


#include "hw_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include "ei_toplevel.h"
#include "ei_widget_func.h"
#include "ei_frame.h"

static void set_screen_location(ei_widget_t, int width, int height, int x_anchor, int y_anchor);
static int default_width( ei_widget_t);
static int default_height( ei_widget_t);


void		ei_place	(ei_widget_t		widget,
                 ei_anchor_t*		anchor,
                 int*			x,
                 int*			y,
                 int*			width,
                 int*			height,
                 float*			rel_x,
                 float*			rel_y,
                 float*			rel_width,
                 float*			rel_height) {
	// If widget is not displayed, allocate memory for placer_params in order to assign values then display it on the screen
	// else hide it from the screen
	if (widget == NULL) return;

	if (widget->placer_params == NULL) {
		widget->placer_params = calloc(1, sizeof(ei_impl_placer_params_t));
		// Valeurs par défaut
		widget->placer_params->anchor = ei_anc_northwest;
	}

	ei_impl_placer_params_t* p = widget->placer_params;
	if (anchor != NULL) p->anchor = *anchor;
	if (x != NULL)      p->x = *x;
	if (y != NULL)      p->y = *y;
	if (width != NULL)  p->width = *width;
	if (height != NULL) p->height = *height;
	if (rel_x != NULL)  p->rel_x = *rel_x;
	if (rel_y != NULL)  p->rel_y = *rel_y;
	if (rel_width != NULL)  p->rel_width = *rel_width;
	if (rel_height != NULL) p->rel_height = *rel_height;

	ei_impl_placer_run(widget);
}

void ei_impl_placer_run(ei_widget_t widget) {

	if (widget == NULL || widget->placer_params == NULL || widget->parent == NULL)
		return;

	// Take screen location to prevent error of toplevel
	ei_rect_t parent_rect = *widget->parent->content_rect;
	if (is_toplevel(widget->parent) && widget == ((ei_impl_toplevel_t*)widget->parent)->title_bar.button) {
		parent_rect = widget->parent->screen_location;
	}

	ei_impl_placer_params_t* params = widget->placer_params;

	// lui-même si c'est le root

	// Calcule de la taille qui prend en compte les priorités
	// Calculé dans ei_impl_run car necessite la taille du parent actualisé
	int width, height;
	if (params->width) {
		width = params->width;
	} else if (params->rel_width) {
		width = (int)(params->rel_width * (float)parent_rect.size.width);
	} else if (widget->requested_size.width) {
		width = widget->requested_size.width;
	} else {
		width = default_width(widget);
	}

	if (params->height) {
		height = params->height;
	} else if (params->rel_height) {
		height = (int)(params->rel_height * (float)parent_rect.size.height);
	} else if (widget->requested_size.height) {
		height = widget->requested_size.height;
	} else {
		height = default_height(widget);
	}

	// Set the position depending on anchor
	int x_anchor = (int)((float)parent_rect.top_left.x + (float)params->x + params->rel_x * (float)parent_rect.size.width);
	int y_anchor = (int)((float)parent_rect.top_left.y + (float)params->y + params->rel_y * (float)parent_rect.size.height);

	switch (params->anchor) {
		case ei_anc_north:
			x_anchor -= width/2;
			break;
		case ei_anc_west:
			y_anchor -= height/2;
			break;
		case ei_anc_center:
			x_anchor -= width/2;
			y_anchor -= height/2;
			break;
		case ei_anc_southwest:
			y_anchor -= height;
			break;
		case ei_anc_south:
			x_anchor -= width/2;
			y_anchor -= height;
			break;
		case ei_anc_east:
			x_anchor -= width;
			y_anchor -= height/2;
			break;
		case ei_anc_northeast:
			x_anchor -= width;
			break;
		case ei_anc_southeast:
			x_anchor -= width;
			y_anchor -= height;
			break;
		default:
			break;
	}

	if (ei_is_toplevelclass(widget)) {
		ei_impl_toplevel_t* toplevel = (ei_impl_toplevel_t*)widget;
		height += toplevel->title_bar.rect.size.height;
	}
	set_screen_location(widget, width, height, x_anchor, y_anchor);
	widget->wclass->geomnotifyfunc(widget);

	ei_widget_t widget_child = widget->children_head;
	while (widget_child != NULL) {
		ei_impl_placer_run(widget_child);
		widget_child = widget_child->next_sibling;
	}
}


int default_width(ei_widget_t widget) {
	int width = 0;
	if (ei_is_toplevelclass(widget)) {
		width = 320 + 2 * widget->border_width;
	}
	else if (ei_is_frameclass(widget) || ei_is_buttonclass(widget)) {
		ei_impl_frame_t* self = (ei_impl_frame_t*) widget;
		if (self->text != NULL) {
			int text_width, text_height;
			hw_text_compute_size(self->text, self->text_font, &text_width, &text_height);
			width += text_width;
		}
		if (self->img != NULL) {
			if (self->img_rect != NULL){
				width += self->img_rect->size.width;
			} else {
				// the whole image is displayed
				ei_size_t image_size = hw_surface_get_size(self->img);
				width += image_size.width;
			}
		}
		if (ei_is_buttonclass(widget)) {
			width += 2 * k_default_button_border_width;
		}
	}
	return width;
}

int default_height(ei_widget_t widget) {
	int height = 0;
	if (ei_is_toplevelclass(widget)) {
		height = 240 + 2 * widget->border_width;
	}
	else if (ei_is_frameclass(widget) || ei_is_buttonclass(widget)) {
		ei_impl_frame_t* self = (ei_impl_frame_t*) widget;
		if (self->text != NULL) {
			int text_width, text_height;
			hw_text_compute_size(self->text, self->text_font, &text_width, &text_height);
			height += text_height;
		}
		if (self->img != NULL) {
			if (self->img_rect != NULL){
				height += self->img_rect->size.height;
			} else {
				// the whole image is displayed
				ei_size_t image_size = hw_surface_get_size(self->img);
				height += image_size.height;
			}
		}
		if (ei_is_frameclass(widget)) {
			height += 2 * widget->border_width;
		} else {
			height += 2 * k_default_button_border_width;
		}
	}
	return height;
}


static void set_screen_location(ei_widget_t widget, int width, int height, int x_anchor, int y_anchor) {
	// TODO calculate the screen location taking into consideration of
	// img, text, anchor, x, y, width, height
	// To make sure all its attributes can be displayed on the screen
	widget->screen_location.top_left = (ei_point_t){x_anchor, y_anchor};
	widget->screen_location.size = (ei_size_t){width, height};
}

void ei_placer_forget(ei_widget_t widget)
{
	// To hide a widget
	// Have to hide all its children
	ei_widget_t curr = widget->children_head;
	while (curr != NULL) {
		ei_placer_forget(curr);
		curr = curr->next_sibling;
	}
	// We dont assign NULL to placer_params as we dont want to free it
	// If we assign it to NULL we will lose the address of the memory allocated
	// which cause the problem of not freeing memory after allocation
	// We will free it in releasefunc()
	if (ei_widget_is_displayed(widget)) {
		ei_place(widget, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}
}

void ei_place_toplevel(ei_widget_t widget) {
	if (widget == NULL) return;
	if (ei_is_toplevelclass(widget)) {
		ei_place_xy(widget,
		    widget->screen_location.top_left.x,
		    widget->screen_location.top_left.y);
	}
	ei_widget_t curr_child = widget->children_head;
	while (curr_child != NULL) {
		ei_place_toplevel(curr_child);
		curr_child = curr_child->next_sibling;
	}
}

