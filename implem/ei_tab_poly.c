//
// Created by kohy on 5/6/25.
//
#include "ei_tab_poly.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

int initialise_TC(ei_point_t* point_array, size_t point_array_size, tab_poly* TC[], int* y_max_glob, int* x_max_glob, int* x_min_glob)
{
    int y_min_glob = point_array[0].y;
    *y_max_glob = point_array[0].y;
    *x_max_glob = point_array[0].x;
    *x_min_glob = point_array[0].x;

    for (int i=0; i < point_array_size-1; i++){
        const ei_point_t point1 = point_array[i];
        const ei_point_t point2 = point_array[i+1];

        if (point1.y != point2.y)
        {
            int xy_min = point1.x;
            int y_max = point2.y, y_min = point1.y;
            int x_max = point2.x, x_min = point1.x;

            if (point1.y > point2.y)
            {
                xy_min = point2.x;
                y_max = point1.y;
                y_min = point2.y;
            }

            if (point1.x > point2.x)
            {
                x_max = point1.x;
                x_min = point2.x;
            }

            if (y_min < y_min_glob) y_min_glob = y_min;
            if (*y_max_glob < y_max) *y_max_glob = y_max;
            if (x_min < *x_min_glob) *x_min_glob = x_min;
            if (*x_max_glob < x_max) *x_max_glob = x_max;

            const double fract = (double) (point1.x - point2.x) / (point1.y - point2.y);

            tab_poly* new_cell = calloc(1, sizeof(tab_poly));
            new_cell->x = xy_min;
            new_cell->y = y_max;
            new_cell->fract = fract;
            new_cell->next = NULL;

            if (TC[y_min] == NULL)
            {
                TC[y_min] = new_cell;
                continue;
            }
            else
            {
                tab_poly* last_cell = TC[y_min];

                while (last_cell->next != NULL)
                {
                    last_cell = last_cell->next;
                }
                last_cell->next = new_cell;
            }
        }
    }
    return y_min_glob;
}
/*
int compare_tab_poly_by_x(const void* a, const void* b) {
    tab_poly* pa = *(tab_poly**)a;
    tab_poly* pb = *(tab_poly**)b;

    if (pa->x < pb->x) return -1;
    if (pa->x > pb->x) return 1;
    return 0;
}

void sort_tca(tab_poly** TCA)
{
    if (*TCA == NULL) return;

    // Step 1: Count nodes
    int count = 0;
    tab_poly* current = *TCA;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    // Step 2: Copy pointers to array
    tab_poly** array = malloc(count * sizeof(tab_poly*));
    current = *TCA;
    for (int i = 0; i < count; i++) {
        array[i] = current;
        current = current->next;
    }

    // Step 3: Sort using qsort
    qsort(array, count, sizeof(tab_poly*), compare_tab_poly_by_x);

    // Step 4: Rebuild linked list
    for (int i = 0; i < count - 1; i++) {
        array[i]->next = array[i + 1];
    }
    array[count - 1]->next = NULL;

    // Step 5: Update head
    *TCA = array[0];

    // Clean up
    free(array);
}
*/
// Old sort

void sort_tca(tab_poly** TCA)
{
    tab_poly *current_cell = *TCA;
    while (current_cell != NULL)
    {
        tab_poly *next_cell = current_cell->next;

        double min_value_x = current_cell->x;
        int min_value_y = current_cell->y;
        double min_value_fract = current_cell->fract;
        tab_poly* min_cell = current_cell;

        while (next_cell != NULL)
        {
            if (next_cell->x < min_value_x)
            {
                min_value_x = next_cell->x;
                min_value_y = next_cell->y;
                min_value_fract = next_cell->fract;
                min_cell = next_cell;
            }
            next_cell = next_cell->next;
        }

        // Swap x and y (not full struct)
        const double temp_x = current_cell->x;
        const int temp_y = current_cell->y;
        const double temp_fract = current_cell->fract;

        current_cell->x = min_value_x;
        current_cell->y = min_value_y;
        current_cell->fract = min_value_fract;

        min_cell->x = temp_x;
        min_cell->y = temp_y;
        min_cell->fract = temp_fract;

        current_cell = current_cell->next;
    }
}

void delete_segment(tab_poly**TCA, tab_poly** prev, tab_poly** curr)
{
    tab_poly* temp = *curr;
    tab_poly* next = temp->next;
    if (*prev == NULL) *TCA = next;
    else (*prev)->next = next;
    temp->next = NULL;
    *curr = next;
    free(temp);
}

void update_xymin(tab_poly** curr)
{
    while ((*curr) != NULL)
    {
        (*curr) -> x += (*curr) -> fract;
        (*curr) = (*curr) -> next;
    }
}