//
// Created by kohy on 5/7/25.
//
#include "ei_button.h"
#include "ei_application.h"
#include "ei_widget_attributes.h"
#include "ei_widget_func.h"

static bool ei_color_equal(ei_color_t c1, ei_color_t c2);

static uint32_t pick_id = 0xFFFFFFFF;

ei_widget_t		ei_widget_create		(ei_const_string_t	class_name,
                             ei_widget_t		parent,
                             ei_user_param_t	user_data,
                             ei_widget_destructor_t destructor) {
    ei_widgetclass_t* widgetclass = ei_widgetclass_from_name(class_name);
    ei_widget_t widget = widgetclass->allocfunc();
    widget->wclass = widgetclass;
    strcpy(widget->wclass->name, class_name);
    widget->parent = parent;
    pick_id -= 10;
    widget->pick_id = pick_id;
    widget->pick_color = ei_pickid_to_pickcolor(widget->pick_id);
    widget->border_width = 0;
    widget->user_data = user_data;
    widget->destructor = destructor;

    widget->children_head = NULL;
    widget->children_tail = NULL;
    widget->last_sibling = NULL;
    widget->next_sibling = NULL;

    // add widget to the parent's children list
    ei_add_child(parent, widget);
    widget->placer_params = NULL;
    widget->content_rect = NULL;

    widget->wclass->setdefaultsfunc(widget);
    return widget;
}

void			ei_widget_destroy		(ei_widget_t		widget) {
	if (widget == NULL) {
        return;
    }
    // Destroys all its descendants
    ei_remove_widget_from_siblings(widget);

    ei_widget_t curr_child = widget->children_head;
    while (curr_child != NULL)
    {
        ei_widget_t next_child = curr_child->next_sibling;
        ei_widget_destroy(curr_child);
        curr_child = next_child;
    }

    // Calls its destructor if it was provided
    if (widget->destructor != NULL) {
        widget->destructor(widget);
    }
    // Frees the memory used by the widget
    widget->wclass->releasefunc(widget);
    free(widget);
}

bool	 		ei_widget_is_displayed		(ei_widget_t		widget) {
    return widget->placer_params != NULL;
}

ei_widget_t		ei_widget_pick			(ei_point_t*		where) {
    ei_surface_t picking_surface = ei_app_picking_surface();
    hw_surface_lock(picking_surface);
    uint32_t* top_left = (uint32_t*) hw_surface_get_buffer(picking_surface);
    hw_surface_unlock(picking_surface);
    ei_size_t size = hw_surface_get_size(picking_surface);
    uint32_t* pixel_adr = top_left + where->y * size.width + where->x;
    ei_color_t pick_color = ei_assign_pick_color(pixel_adr);
    return ei_find_widget_from_pickcolor(ei_app_root_widget(), pick_color);
}

static bool ei_color_equal(ei_color_t c1, ei_color_t c2) {
    return c1.red == c2.red &&
           c1.green == c2.green &&
           c1.blue == c2.blue &&
               (c1.alpha != false && c2.alpha != false ? c1.alpha == c2.alpha : true);
}

ei_widget_t ei_find_widget_from_pickcolor(ei_widget_t widget, ei_color_t pick_color) {
    // Check the widget itself
    if (ei_color_equal(widget->pick_color, pick_color)) {
        return widget;
    }
    // Check all the children
    ei_widget_t child = widget->children_head;
    while (child != NULL) {
        // picking is only useful for the main window
        // so no need to check the widget not displayed
        if (ei_widget_is_displayed(child)) {
            ei_widget_t result = ei_find_widget_from_pickcolor(child, pick_color);
            if (result != NULL && result != ei_app_root_widget()) {
                return result;
            }
        }
        child = child->next_sibling;
    }
    return NULL;
}

ei_color_t ei_assign_pick_color(uint32_t* pixel_adr) {
    if (pixel_adr == NULL) return (ei_color_t){0, 0, 0, 0};
    int ir, ig, ib, ia;
    hw_surface_get_channel_indices(ei_app_picking_surface(), &ir, &ig, &ib, &ia);
    ei_color_t pick_color;

    uint8_t* pixel_bytes = (uint8_t*)pixel_adr;
    pick_color.red   = pixel_bytes[ir];
    pick_color.green = pixel_bytes[ig];
    pick_color.blue  = pixel_bytes[ib];
    pick_color.alpha = pixel_bytes[ia];

    return pick_color;
}

void ei_add_child(ei_widget_t parent, ei_widget_t child) {
    if (parent == NULL || child == NULL)
        return;

    if (parent->children_head == NULL)
    {
        parent->children_head = child;
        parent->children_tail = child;
    }
    else
    {
        ei_widget_t last_child = ei_widget_get_last_child(parent);
        last_child->next_sibling = child;
        parent->children_tail = child;
        child->last_sibling = last_child;
    }
}


void ei_remove_widget_from_siblings(ei_widget_t widget) {
    // If widget has last sibling
    // then the next sibling of the widget becomes the next sibling of the widget's last sibling
    // else the next sibling of the widget becomes the children head
    if (widget->last_sibling != NULL)
        widget->last_sibling->next_sibling = widget->next_sibling;
    else{
        if (widget->parent != NULL){
            widget->parent->children_head = widget->next_sibling;
        }
    }
    // If widget has next sibling
    // then the last sibling of the widget becomes the last sibling of the widget's next sibling
    // else the last sibling of the widget becomes the children tail
    if (widget->next_sibling != NULL)
        widget->next_sibling->last_sibling = widget->last_sibling;
    else
        if (widget->parent != NULL){
            widget->parent->children_tail = widget->last_sibling;
        }
    widget->last_sibling = NULL;
    widget->next_sibling = NULL;
}

ei_color_t ei_pickid_to_pickcolor(uint32_t pickid)
{
    int ir, ig, ib, ia;
    hw_surface_get_channel_indices(ei_app_root_surface(), &ir, &ig, &ib, &ia);
    ei_color_t color;
    color.red   = (pickid >> 8*ir) & 0xFF;
    color.green = (pickid >> 8*ig) & 0xFF;
    color.blue  = (pickid >> 8*ib)  & 0xFF;
    color.alpha = (pickid >> 8*ia) & 0xFF;
    return color;
}

void		ei_impl_widget_draw_children	(ei_widget_t		widget,
                         ei_surface_t		surface,
                         ei_surface_t		pick_surface,
                         ei_rect_t*		clipper) {
    if (widget == NULL) return;
    // TODO add clipper
    ei_widget_t child = ei_widget_get_first_child(widget);
    while (child != NULL) {
        ei_rect_t clipper_child = ei_intersection_parent_child(*clipper, *child->content_rect);
        child->wclass->drawfunc(child, surface, pick_surface, &clipper_child);
        ei_widget_t next_siblings = ei_widget_get_next_sibling(child);
        ei_impl_widget_draw_children(child, surface, pick_surface, &clipper_child);
        child = next_siblings;
    }
}

ei_rect_t ei_intersection_parent_child(ei_rect_t parent_rect, ei_rect_t child_rect) {
    ei_rect_t clipper_child;
    int x1 = (int)fmax(parent_rect.top_left.x, child_rect.top_left.x);
    int y1 = (int)fmax(parent_rect.top_left.y, child_rect.top_left.y);
    int x2 = (int)fmin(parent_rect.top_left.x + parent_rect.size.width, child_rect.top_left.x + child_rect.size.width);
    int y2 = (int)fmin(parent_rect.top_left.y + parent_rect.size.height, child_rect.top_left.y + child_rect.size.height);

    if (x2 > x1 && y2 > y1) {
        //child dans le widget
        clipper_child.top_left.x = x1;
        clipper_child.top_left.y = y1;
        clipper_child.size.width = x2 - x1;
        clipper_child.size.height = y2 - y1;
    } else {
        //child hors content_rect de widget
        clipper_child.top_left.x = 0;
        clipper_child.top_left.y = 0;
        clipper_child.size.width = 0;
        clipper_child.size.height = 0;
    }
    return clipper_child;
}

bool is_point_on_rect(ei_rect_t rect, ei_point_t point) {
    return point.x >= rect.top_left.x && point.x <= rect.top_left.x + rect.size.width &&
        point.y >= rect.top_left.y && point.y <= rect.top_left.y + rect.size.height;
}

void ei_destroy_parent(ei_widget_t widget){
    ei_widget_destroy(widget->parent);
}

bool is_same_rect(ei_rect_t rect1, ei_rect_t rect2) {
    return is_same_size(rect1.size, rect2.size) && is_same_point(rect1.top_left, rect2.top_left);
}

bool is_same_point(ei_point_t pt1, ei_point_t pt2) {
    return pt1.x == pt2.x && pt1.y == pt2.y;
}

bool is_same_size(ei_size_t size1, ei_size_t size2) {
    return size1.width == size2.width && size1.height == size2.height;
}

ei_widget_t ei_get_topmost_window(ei_widget_t widget) {
    ei_widget_t topmost = ei_widget_get_last_child(widget);
    while (topmost != NULL) {
        if (ei_is_toplevelclass(topmost)) {
            return topmost;
        }
        topmost = topmost->last_sibling;
    }
    return NULL;
}
