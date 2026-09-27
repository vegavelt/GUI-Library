//
// Created by youchen on 5/9/25.
//


#include "ei_application.h"
#include "ei_button.h"

typedef struct wclass_register {
    ei_widgetclass_t* head;
    ei_widgetclass_t* tail;
}wclass_register;

static wclass_register wregister = {NULL, NULL};

size_t		ei_widget_struct_size() {
    return sizeof(ei_impl_widget_t);
}

void			ei_widgetclass_register		(ei_widgetclass_t* widgetclass)
{
    // assign NULL to widgetclass to prevent
    // random memory address left over from uninitialized stack or heap memory,
    // leading to a segfault the moment it's dereferenced.
    widgetclass->next = NULL;
    if (wregister.head == NULL)
    {
        wregister.head = widgetclass;
        wregister.tail = widgetclass;
    }
    else
    {
        wregister.tail->next = widgetclass;
        wregister.tail = wregister.tail->next;
    }
}


ei_widgetclass_t*	ei_widgetclass_from_name	(ei_const_string_t name)
{
    ei_widgetclass_t* curr = wregister.head;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }
	return NULL;
}

bool ei_is_widgetclass_in_register(ei_widget_t widget) {
    ei_widgetclass_t* curr = wregister.head;
    while (curr != NULL) {
        if (widget->wclass == curr) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

void ei_widgetclass_destroy()
{
    ei_widgetclass_t* curr = wregister.head;
    while (curr != NULL) {
        ei_widgetclass_t* next = curr->next;
        free(curr);
        curr = next;
    }
}

bool ei_is_toplevelclass(ei_widget_t widget) {
    if (widget == NULL) return false;
    return widget->wclass == ei_widgetclass_from_name("toplevel");
}

bool ei_is_frameclass(ei_widget_t widget) {
    if (widget == NULL) return false;
    return widget->wclass == ei_widgetclass_from_name("frame");
}

bool ei_is_buttonclass(ei_widget_t widget) {
    if (widget == NULL) return false;
    return widget->wclass == ei_widgetclass_from_name("button");
}
