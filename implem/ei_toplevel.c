//
// Created by kohy on 5/12/25.
//

#include "ei_toplevel.h"
#include "ei_button.h"
#include "ei_widget_attributes.h"
#include "ei_widget_func.h"
#include "ei_default.h"
#include "ei_event_func.h"
#include "ei_application.h"

static ei_widget_t ei_toplevel_alloc();
static void	ei_toplevel_release		(ei_widget_t	widget);
static void	ei_toplevel_draw	(ei_widget_t		widget,
							 ei_surface_t		surface,
							 ei_surface_t		pick_surface,
							 ei_rect_t*		clipper);
static void	ei_toplevel_setdefaults	(ei_widget_t		widget);
static void	ei_toplevel_geomnotify	(ei_widget_t		widget);
static bool	ei_toplevel_handle		(ei_widget_t		widget,
						 	 struct ei_event_t*	event);
static void ei_set_rect_title_bar(ei_widget_t widget);
static void ei_set_resize_zone(ei_widget_t widget);

void ei_init_toplevelclass(ei_widgetclass_t* wclass)
{
	strcpy(wclass->name, "toplevel");
  	wclass->allocfunc = ei_toplevel_alloc;
	wclass->releasefunc = ei_toplevel_release;
	wclass->drawfunc = ei_toplevel_draw;
	wclass->setdefaultsfunc = ei_toplevel_setdefaults;
	wclass->geomnotifyfunc = ei_toplevel_geomnotify;
	wclass->handlefunc = ei_toplevel_handle;
}


static ei_widget_t ei_toplevel_alloc()
{
	return malloc(sizeof(ei_impl_toplevel_t));
}

static void	ei_toplevel_setdefaults	(ei_widget_t		widget)
{
	widget->requested_size = ei_toplevel_default_requested_size;
	widget->border_width = 4;
	widget->background_color = ei_default_background_color;

	ei_impl_toplevel_t* self = (ei_impl_toplevel_t*)widget;
	self->closable = true;
	self->resizable = ei_axis_both;
	self->min_size = (ei_size_t*)&ei_toplevel_default_min_size;

	self->title_bar.title = ei_default_title;

	self->title_bar.rect.size.height = ei_default_title_bar_height;
	self->title_bar.rect.size.width = ei_calculate_min_size(widget);
	self->title_bar.rect.top_left = (ei_point_t){0, 0};
	ei_create_button_title_bar(widget);

	self->resize_zone.size.height = 4*ei_default_margin;
	self->resize_zone.size.width = 4*ei_default_margin;
	self->resize_zone.top_left = (ei_point_t){0, 0};
}


void			ei_toplevel_configure		(ei_widget_t		widget,
			     ei_size_t*		requested_size,
			     const ei_color_t*	color,
			     int*			border_width,
			     ei_string_t*		title,
			     bool*			closable,
			     ei_axis_set_t*		resizable,
			      ei_size_ptr_t*		min_size)
{
	ei_impl_toplevel_t*	self = (ei_impl_toplevel_t*)widget;
	if (requested_size != NULL) self->widget.requested_size = *requested_size;
	if (color != NULL) self->widget.background_color = *color;
	if (border_width != NULL) self->widget.border_width = *border_width;

	if (title != NULL) {
		if (strcmp(self->title_bar.title, ei_default_title) != 0) {
			free(self->title_bar.title);
		}
		if (strcmp(*title, ei_default_title) != 0) {
			self->title_bar.title = malloc(strlen(*title) + 1);
			strcpy(self->title_bar.title, *title);
		} else {
			self->title_bar.title = ei_default_title;
		}
	}
	if (closable != NULL) self->closable = *closable;
	if (resizable != NULL) self->resizable = *resizable;
	if (min_size != NULL) self->min_size = *min_size;

	// If toplevel non closable and button exists
	// then destroy the button
	// else if toplevel closable and button not exists
	// then create the button
	if (!self->closable) {
		if (self->title_bar.button != NULL) {
			ei_destroy_close_button(widget);
		}
	} else if (self->title_bar.button == NULL) {
		ei_create_button_title_bar(widget);
	}
}


static void	ei_toplevel_draw	(ei_widget_t		widget,
							 ei_surface_t		surface,
							 ei_surface_t		pick_surface,
							 ei_rect_t*		clipper) {
	ei_impl_toplevel_t* self = (ei_impl_toplevel_t*)widget;
	// Draw the toplevel on picking surface, no need to draw one by one as a rectangle is enough
	ei_fill(pick_surface, &widget->pick_color, &widget->screen_location);

	// 1 - Draw the title bar
	ei_rect_t title_bar_rect = self->title_bar.rect;
	title_bar_rect.size.width += 2 * self->widget.border_width;
	title_bar_rect.top_left.x -= self->widget.border_width;

	int array_points_size = 0;

	ei_color_t top_bar_color = {100,100,100,255};

	ei_point_t* rounded_points = rounded_top_corners(title_bar_rect,5,&array_points_size);
	ei_draw_polygon(surface, rounded_points, array_points_size, top_bar_color, &title_bar_rect);
	ei_rect_t border_rect = *widget->content_rect;

	if (self->widget.border_width > 0)
	{
		border_rect.size.width += 2 * self->widget.border_width;
		border_rect.top_left.x -= self->widget.border_width;
		border_rect.size.height += self->widget.border_width;
		ei_fill(surface, &top_bar_color, &border_rect);
	}
	//Draw the border window
	free(rounded_points);

	ei_rect_t clipper_toplevel;
	// 2 - Draw the button on title bar
	if (self->title_bar.button != NULL){
    		clipper_toplevel = ei_intersection_parent_child(self->title_bar.rect, self->title_bar.rect);
		ei_widgetclass_from_name("button")->drawfunc(self->title_bar.button, surface, pick_surface, &clipper_toplevel);
	}

	if (self->title_bar.title != NULL) {
		ei_font_t font = ei_default_font;
		int width = 0, height = 0;
		hw_text_compute_size(self->title_bar.title, font, &width, &height);

		ei_rect_t rectangle = self->title_bar.rect;
		ei_point_t widget_center = {rectangle.size.width/2, rectangle.size.height/2};

		int x_where = rectangle.top_left.x, y_where = rectangle.top_left.y;
		int text_width = 0, text_height = 0;

		hw_text_compute_size (self->title_bar.title, font, &text_width, &text_height);
		ei_point_t text_center = {text_width/2,text_height/2};
		ei_text_anchor_coordinate(ei_anc_west, &x_where, &y_where, text_center, widget_center);

		ei_point_t where = {x_where, y_where};
		if (self->title_bar.button != NULL) {
			where.x += self->title_bar.button->screen_location.size.width + 2 * ei_default_margin;
		}

		ei_color_t text_color = { 255, 255, 255, 255 };
		ei_rect_t dst_rect = {where, {width, height}};
		ei_rect_t image_rect = intersection_clipper(dst_rect, NULL, &self->title_bar.rect);
		ei_draw_text(surface, &where, self->title_bar.title, font, text_color, &image_rect);
	}

	// 3 - Draw the content rectangle
	ei_fill(surface, &widget->background_color, widget->content_rect);

	// 4 - Draw the resize zone
	if (self->resizable != ei_axis_none) {
		clipper_toplevel = ei_intersection_parent_child(*widget->content_rect, self->resize_zone); //rect pour redimensionner
		ei_fill(surface, &top_bar_color, &clipper_toplevel);
	}

}

static void	ei_toplevel_geomnotify	(ei_widget_t		widget)
{
	ei_impl_toplevel_t* self = (ei_impl_toplevel_t*)widget;

	widget->screen_location.size = get_min_size((ei_size_t){self->min_size->width, self->min_size->height}, (ei_size_t){widget->screen_location.size.width, widget->screen_location.size.height});
	ei_widget_set_content_rect(widget, &widget->screen_location);
	ei_rect_t content_rect = widget->screen_location;

	//les 2 lignes d'après décalent et ducoup le clipper calcule le boutton hors zone et donc l'aafiche pas
	content_rect.top_left.y += self->title_bar.rect.size.height;
	content_rect.size.height -= self->title_bar.rect.size.height;

	ei_widget_set_content_rect(widget, &content_rect);
	if (self->resizable != ei_axis_none) ei_set_resize_zone(widget);
	ei_set_rect_title_bar(widget);

	// If it is a toplevel, replace the close button too
	if (self->title_bar.button != NULL && !ei_widget_is_displayed(self->title_bar.button)) {
		int x = ei_default_margin, y = ei_default_margin;
		ei_place(self->title_bar.button,
		&(ei_anchor_t){ei_anc_northwest},
		&x, &y,
		NULL, NULL,
		NULL, NULL,
		NULL, NULL);
	}
}

static bool	ei_toplevel_handle		(ei_widget_t		widget,
						 	 struct ei_event_t*	event)
{
	// Case 1 : Click on the toplevel will make it the children_head among its siblings change the siblings too
	// Case 2 : Move the toplevel
	// Case 3 : Resize the toplevel

	bool event_handled = false;
	ei_widget_t active_widget = ei_event_get_active_widget();
	ei_impl_toplevel_t* toplevel = active_widget == NULL ? (ei_impl_toplevel_t*)widget : (ei_impl_toplevel_t*)active_widget;
	if (toplevel == NULL) return false;
	if (toplevel->widget.wclass == NULL) return false;

	ei_widget_t curr_widget = (ei_widget_t)toplevel;

	// Conditions to handle different event
	bool is_over_toplevel = is_point_on_rect(toplevel->widget.screen_location, event->param.mouse.where);
	bool is_over_title_bar = is_point_on_rect(toplevel->title_bar.rect, event->param.mouse.where);
	bool is_on_resize_zone = is_point_on_rect(toplevel->resize_zone, event->param.mouse.where);

	switch (event->type) {
		case ei_ev_mouse_buttondown:
			if (is_over_toplevel && is_left_m_button(*event)) {
				// Put it to the children tail (last to draw)
				ei_event_set_active_widget(curr_widget);
				if (is_on_resize_zone) ei_event_set_state(ei_toplevel_resize);
				else if (is_over_title_bar) ei_event_set_state(ei_toplevel_drag);
				ei_event_set_mouse_initial_position(event->param.mouse.where);

				// So it will be on top of the window
				ei_remove_widget_from_siblings(curr_widget);
				ei_add_child(curr_widget->parent, curr_widget);
				event_handled = true;
			}
			break;

		case ei_ev_mouse_buttonup:
			// No need to return true as widget attributes are not changed
			ei_event_set_mouse_initial_position((ei_point_t){0, 0});
			ei_event_set_state(ei_none);
			ei_event_set_active_widget(NULL);
			break;

		case ei_ev_mouse_move:
			if (is_widget_active() && mouse_on_parent(curr_widget, *event)) {
				// Replace it to the new screen location
				ei_point_t initial_m_pt = ei_event_get_mouse_initial_position();
				ei_point_t mouse_dist = event->param.mouse.where;
				mouse_dist.x -= initial_m_pt.x;
				mouse_dist.y -= initial_m_pt.y;
				switch (ei_event_get_state()){
					case ei_toplevel_resize:
						switch (toplevel->resizable) {
							case ei_axis_x:
								ei_place_size(curr_widget,
								&(int){curr_widget->content_rect->size.width + mouse_dist.x},
									NULL);
								event_handled = true;
								break;
							case ei_axis_y:
								ei_place_size(curr_widget,
								NULL,
								&(int){curr_widget->content_rect->size.height + mouse_dist.y});
								event_handled = true;
								break;
							case ei_axis_both:
								ei_place_size(curr_widget,
								&(int){curr_widget->content_rect->size.width + mouse_dist.x},
								&(int){curr_widget->content_rect->size.height + mouse_dist.y});
								event_handled = true;
								break;
							default:
								break;
						}
						break;

					case ei_toplevel_drag:
						ei_place_xy(curr_widget,
							curr_widget->screen_location.top_left.x + mouse_dist.x,
							curr_widget->screen_location.top_left.y + mouse_dist.y);
						event_handled = true;
						break;
					default:
						break;
				}
				ei_event_set_mouse_initial_position(event->param.mouse.where);
			}
			break;
		default:
			break;
	}
	return event_handled;
}


static void	ei_toplevel_release		(ei_widget_t	widget)
{
	if (ei_widget_is_displayed(widget)) free(widget->placer_params);
	if (!is_same_rect(*widget->content_rect, widget->screen_location)) free(widget->content_rect);
	ei_impl_toplevel_t*	self = (ei_impl_toplevel_t*)widget;
	if (self->title_bar.title != ei_default_title) free(self->title_bar.title);
}

int ei_calculate_min_size(ei_widget_t widget) {
	ei_impl_toplevel_t* self = (ei_impl_toplevel_t*)widget;
	return self->widget.requested_size.height < self->min_size->height ? self->min_size->height : self->widget.requested_size.height;
}

static void ei_set_rect_title_bar(ei_widget_t widget) {
	// calculate the screen location taking into consideration of
	// img, text, anchor, x, y, width, height
	// To make sure all its attributes can be displayed on the screen
	ei_impl_toplevel_t*	self = (ei_impl_toplevel_t*)widget;
	self->title_bar.rect.size.width = widget->screen_location.size.width;
	self->title_bar.rect.top_left.x = widget->screen_location.top_left.x;
	self->title_bar.rect.top_left.y = widget->screen_location.top_left.y;
}

static void ei_set_resize_zone(ei_widget_t widget) {
	ei_impl_toplevel_t*	self = (ei_impl_toplevel_t*)widget;
	self->resize_zone.top_left.x = widget->content_rect->top_left.x + widget->content_rect->size.width - self->resize_zone.size.width;
	self->resize_zone.top_left.y = widget->content_rect->top_left.y + widget->content_rect->size.height - self->resize_zone.size.height;
}

ei_size_t get_min_size(ei_size_t min_size, ei_size_t size) {
	ei_size_t new_size = min_size;
	if (size.width > min_size.width) new_size.width = size.width;
	if (size.height > min_size.height) new_size.height = size.height;
	return new_size;
}

void ei_destroy_close_button(ei_widget_t widget) {
	ei_impl_toplevel_t* self = (ei_impl_toplevel_t*)widget;
	ei_widget_destroy(self->title_bar.button);
	self->title_bar.button = NULL;
}


bool is_toplevel(ei_widget_t widget) {
	if (widget == NULL || widget == ei_app_root_widget()) {
		return false;
	}
	if (ei_is_toplevelclass(widget)) {
		return true;
	}
	return is_toplevel(widget->parent);
}

ei_widget_t get_toplevel_parent(ei_widget_t widget) {
	if (widget == ei_app_root_widget() || widget == NULL) {
		return NULL;
	}
	if (ei_is_toplevelclass(widget)) {
		return widget;
	}
	return get_toplevel_parent(widget->parent);
}