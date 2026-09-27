//
// Created by guyal on 5/6/25.
//

#ifndef EI_TAB_POLY_H
#define EI_TAB_POLY_H
#include "ei_types.h"

typedef struct tab_poly tab_poly;

// polygon array that contains y, x, fraction and pointer to next
struct tab_poly {
  int y;
  double x;
  double fract;
  tab_poly* next;
};

/**
 * @brief	Initialise the array TC that contains pointers to struct tab_poly.
 *		Pixels are ordered by horizontal lines,
 *		from top to bottom, and from left to right within lines.
 *
 * @param 	surface		The surface from which the pixel address is returned.
 */
int initialise_TC(ei_point_t* point_array, size_t point_array_size, tab_poly* TC[], int* y_max_glob, int* x_max_glob, int* x_min_glob);

/**
 * @brief Sorts an array of tab_poly structures by the `x` field in ascending order.
 *
 * This function uses a variation of the bubble sort algorithm (often called
 * gnome sort) to sort the elements of the `TCA` array in-place based on
 * the value of the `x` member of each `tab_poly` structure.
 *
 * @param TCA An array of tab_poly structures to be sorted.
 * @param size_tca The number of elements in the TCA array.
 */
void sort_tca(tab_poly** TCA);

/**
 * \brief       Updates the x-coordinate of each node in a linked list of the TCA, which is a `tab_poly` structure. It is used to calculate where
 *              the points of a polygon will be placed when the y-coordinates are incremented by 1.
 *
 *
 * \param  curr  A pointer to a pointer to the first `tab_poly` structure in the linked list.
 */

void update_xymin(tab_poly** curr);


/**
 * @brief delete a cell in a list. Current cell becomes next cell if next is not NULL
 * else current becomes NULL
 *
 * @param prev a tab_poly structure
 * @param curr a tab_poly structure
 * @param next a tab_poly structure
 */
void delete_segment(tab_poly**TCA, tab_poly** prev, tab_poly** curr);


#endif //EI_TYPES_IMPLEM_H
