//
// Created by kohy on 5/12/25.
//

#include "ei_default.h"
#include "ei_widget_attributes.h"
#include "ei_button.h"

static ei_widget_t ei_frame_alloc();
static void	ei_frame_release		(ei_widget_t	widget);
static void	ei_frame_draw	(ei_widget_t		widget,
							 ei_surface_t		surface,
							 ei_surface_t		pick_surface,
							 ei_rect_t*		clipper);
static void	ei_frame_setdefaults	(ei_widget_t		widget);
static void	ei_frame_geomnotify	(ei_widget_t		widget);
static bool	ei_frame_handle		(ei_widget_t		widget,
						 	 struct ei_event_t*	event);

static bool are_sizes_equal(ei_size_t size1, ei_size_t size2);

static bool are_colors_equal(ei_color_t color1, ei_color_t color2);

void ei_init_frameclass(ei_widgetclass_t* wclass)
{
	strcpy(wclass->name, "frame");
  	wclass->allocfunc = ei_frame_alloc;
	wclass->releasefunc = ei_frame_release;
	wclass->drawfunc = ei_frame_draw;
	wclass->setdefaultsfunc = ei_frame_setdefaults;
	wclass->geomnotifyfunc = ei_frame_geomnotify;
	wclass->handlefunc = ei_frame_handle;
}


static ei_widget_t ei_frame_alloc()
{
	return malloc(sizeof(ei_impl_frame_t));
}


static void	ei_frame_setdefaults	(ei_widget_t		widget)
{
	widget->requested_size = ei_frame_default_requested_size;
	widget->border_width = ei_frame_default_border_width;
	widget->background_color = ei_default_background_color;
	widget->content_rect = &widget->screen_location;

	ei_impl_frame_t* self = (ei_impl_frame_t*) widget;
	self->relief = ei_relief_none;
	self->text = NULL;
	self->text_font = ei_default_font;
	self->text_color = ei_font_default_color;
	self->text_anchor = ei_anc_center;
	self->img = NULL;
	self->img_rect = NULL;
	self->img_anchor = ei_anc_center;
}


void	ei_frame_configure		(ei_widget_t		widget,
							 ei_size_t*		requested_size,
							 const ei_color_t*	color,
							 int*			border_width,
							 ei_relief_t*		relief,
							 ei_string_t*		text,
							 ei_font_t*			text_font,
							 ei_color_t*		text_color,
							 ei_anchor_t*		text_anchor,
							 ei_surface_t*		img,
							 ei_rect_ptr_t*		img_rect,
							 ei_anchor_t*		img_anchor)
{
	if (requested_size != NULL) widget->requested_size = *requested_size;
	if (color != NULL) widget->background_color = *color;
	if (border_width != NULL) widget->border_width = *border_width;

	ei_impl_frame_t*	self = (ei_impl_frame_t*)widget;
	if (relief != NULL) self->relief = *relief;
	if (text != NULL && *text != NULL) {
		if (self->text == NULL || strlen(self->text) == 0) {
			free(self->text);
			self->text = malloc(strlen(*text) + 1);
		}
		strcpy(self->text, *text);
	}
	if (text_font != NULL) self->text_font = *text_font;
	if (text_color != NULL) self->text_color = *text_color;
	if (text_anchor != NULL) self->text_anchor = *text_anchor;

	if (img_rect != NULL && *img_rect != NULL)
	{
		self->img_rect = malloc(sizeof(ei_rect_t));
		*self->img_rect = **img_rect;
	}

	if (img != NULL)
	{
		if (self->img != NULL) hw_surface_free(self->img);
		if (*img != NULL) {
			ei_surface_t  copy_image = hw_surface_create(*img, hw_surface_get_size(*img),false);
			ei_rect_t copy_image_rect = hw_surface_get_rect(copy_image);
			ei_copy_surface(copy_image, &copy_image_rect, *img, &copy_image_rect, true);
			self->img = copy_image;
		} else {
			self->img = NULL;
		}
	}
	if (img_anchor != NULL) self->img_anchor = *img_anchor;
}


static void	ei_frame_draw	(ei_widget_t			widget,
							 ei_surface_t		surface,
							 ei_surface_t		pick_surface,
							 ei_rect_t*		clipper)
{
	ei_fill(surface, &widget->background_color, clipper);
	ei_fill(pick_surface, &widget->pick_color, clipper);
	ei_impl_frame_t* self = (ei_impl_frame_t*)widget;
	ei_rect_t rectangle = self->widget.screen_location;

	if (self->text != NULL && strlen(self->text) != 0) {
		ei_font_t font = self->text_font;

		if (self->text_font == NULL) font = ei_default_font;
		ei_anchor_t text_anchor = self->text_anchor;

		int width = 0, height = 0;
		hw_text_compute_size(self->text, font, &width, &height);

		ei_point_t widget_center = {rectangle.size.width/2, rectangle.size.height/2};

		int x_where = rectangle.top_left.x, y_where = rectangle.top_left.y;
		int text_width = 0, text_height = 0;

		hw_text_compute_size (self->text, font, &text_width, &text_height);

		ei_point_t text_center = {text_width/2,text_height/2};
		ei_text_anchor_coordinate(text_anchor, &x_where, &y_where, text_center, widget_center);
		ei_point_t where = {x_where, y_where};
		ei_draw_text(surface, &where, self->text, self->text_font, self->text_color, NULL);
	} else if (self->img != NULL){
		ei_anchor_t image_anchor = self->img_anchor;
	        ei_rect_ptr_t image_rect = self->img_rect;

		if (image_rect == NULL) {
			image_rect = malloc(sizeof(ei_rect_t));
			*image_rect = hw_surface_get_rect(self->img);
	        }

		ei_surface_t image = self->img;
		int x_where = rectangle.top_left.x, y_where = rectangle.top_left.y;

		ei_point_t widget_center = {rectangle.size.width/2, rectangle.size.height/2};
		ei_point_t image_center = {image_rect->size.width / 2, image_rect->size.height / 2,};

		ei_text_anchor_coordinate(image_anchor, &x_where, &y_where, image_center, widget_center);

		ei_rect_t dst_rect = {{x_where, y_where}, image_rect->size};
		ei_rect_t image_rect_clipper = rectangle;
		image_rect_clipper.top_left = image_rect->top_left;
		*image_rect = intersection_clipper(*image_rect, NULL, &image_rect_clipper);
		ei_copy_surface(surface, &dst_rect, image ,image_rect, true);
	}
}


static void	ei_frame_geomnotify	(ei_widget_t		widget)
{
	ei_widget_set_content_rect(widget, ei_widget_get_screen_location(widget));
}


static bool	ei_frame_handle		(ei_widget_t		widget,
						 	 struct ei_event_t*	event)
{
	return false;
}


static void	ei_frame_release		(ei_widget_t	widget)
{
	if (widget == NULL) return;
	if (ei_widget_is_displayed(widget)) free(widget->placer_params);
	ei_impl_frame_t* self = (ei_impl_frame_t*)widget;
	if (!is_same_rect(*widget->content_rect, widget->screen_location)) free(widget->content_rect);
	free(self->text);
	if (self->img_rect) free(self->img_rect);
}


bool are_sizes_equal(const ei_size_t size1, const ei_size_t size2)
{
	return size1.width == size2.width && size1.height == size2.height;
}


bool are_colors_equal(const ei_color_t color1, const ei_color_t color2)
{
	return color1.red == color2.red && color1.green == color2.green && color1.blue == color2.blue && color1.alpha == color2.alpha;
}