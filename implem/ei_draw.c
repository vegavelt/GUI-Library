/**
 *  @file	ei_draw.c
 *  @brief
 *
 */

//
// Created by
//
#include "ei_types.h"
#include "ei_tab_poly.h"
#include <stdbool.h>
#include "ei_implementation.h"
//
// Created by kohy on 5/6/25.
//

typedef struct {
    int red;
    int green;
    int blue;
    int alpha;
} RGBAIndex;

typedef struct {
    int mod_x, mod_y;
    int sign_x, sign_y;
    int *E, *x, *y;
    ei_size_t surface_size;
    RGBAIndex color_index;
    uint8_t *pixel_color;
    uint32_t *surface_buffer;
    const ei_rect_t *clipper;
    uint8_t alpha;
}ei_Bresenham_params;

/**
 * \brief	Paints a pixel with a selected color.
 *
 * @param	x  	            x-coordinate of the pixel to paint.
 *
 * @param	y  	            y-coordinate of the pixel to paint.
 *
 * @param	width 	     	Width the surface of in which the pixel to paint is located.
 *
 * @param	color_index     RGBAIndex structure containing the indices for the red, green, blue, and alpha channels.
 *
 * @param	surface_buffer	Pointer to the address of the pixel at coordinated (0, 0) of a surface
 *
 * @param	pixel_color     Array of 4 uint8_t values representing the pixel color,
 *                          where each index is defined by the corresponding entry in color_index
 *
 * @param	alpha           If true, the function will calculate the transparency of the pixel. If false, it will not.
 *
 */
static void ei_draw_pixel(int32_t x,int32_t y, int width, RGBAIndex color_index, uint32_t* surface_buffer, uint8_t pixel_color[4], uint8_t alpha_val);

/**
 * \brief	Uses Bresenham algorithm to draw a line if the slope of it smaller than 1 in absolute value.
 *
 * @param	point  	        Last point of the line.
 *
 * @param	params  	    Pointer to an ei_Bresenham_params structure, containing all the information to execute the Bresenham algorithm.
 */
static void ei_draw_line_x(ei_point_t point, ei_Bresenham_params *params);

/**
 * \brief	Calculates the x and y coordinates of the next iteration of the Bresenham when the slope is greater than 1 in absolute value
 *
 * @param	params  	    Pointer to an ei_Bresenham_params structure, containing all the information to execute the Bresenham algorithm.
 */
static void update_draw_line_values_x(ei_Bresenham_params *params);

/**
 * \brief	Uses Bresenham algorithm to draw a line if the slope of it greater than 1 in absolute value.
 *
 * @param	point  	        Last point of the line.
 *
 * @param	params  	    Pointer to an ei_Bresenham_params structure, containing all the information to execute the Bresenham algorithm.
 */
static void ei_draw_line_y(ei_point_t point, ei_Bresenham_params *params);

/**
 * \brief	Calculates the x and y coordinates of the next iteration of the Bresenham when the slope is smaller than 1 in absolute value
 *
 * @param	params  	    Pointer to an ei_Bresenham_params structure, containing all the information to execute the Bresenham algorithm.
 */
static void update_draw_line_values_y(ei_Bresenham_params *params);

/**
 * \brief	Uses Bresenham algorithm to draw a line if the slope of it greater than 1 in absolute value.
 *
 * @param	point  	        Last point of the line.
 *
 * @param	params  	    Pointer to an ei_Bresenham_params structure, containing all the information to execute the Bresenham algorithm.
 */
static void ei_polygon_fill_color(ei_surface_t surface, uint8_t pixel_color[4], int j, tab_poly *TCA, RGBAIndex color_index, uint8_t alpha, ei_point_t offset);


/**
 * \brief	Sets the values of a pixel to the correct colors and creates an RGBAIndex structure with
 *          the R, G, B, and Alpha channel indices of a surface.
 *
 * @param	surface         The surface where the user will draw.
 *
 * @param	color           The color to which we want to set the pixel.
 *
 * 0param	pixel_color     Pointer to the pixel whose colors we want to set.
 */
RGBAIndex assign_color(ei_surface_t surface, const ei_color_t* color, uint8_t(* pixel_color)[4])
{
    int ir, ig, ib, ia;
    hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);
    (*pixel_color)[ir] = color -> red;
    (*pixel_color)[ig] = color -> green;
    (*pixel_color)[ib] = color -> blue;
    (*pixel_color)[ia] = color -> alpha;

    RGBAIndex index = { .red = ir, .green = ig, .blue = ib, .alpha = ia };
    return index;
}


void ei_draw_pixel(const int32_t x, const int32_t y, const int width, const RGBAIndex color_index, uint32_t* surface_buffer, uint8_t pixel_color[4], uint8_t alpha_val)
{
    uint32_t* pixel_ptr = surface_buffer + y * width + x;
    //uint8_t* surface_current_color = (uint8_t*) pixel_ptr;
    uint8_t* new_pixel_color = (uint8_t*)pixel_ptr;
    if (alpha_val!=255)
    {
        new_pixel_color[color_index.red] = (alpha_val * pixel_color[color_index.red] + (255 - alpha_val) * new_pixel_color [color_index.red]) / 255;
        new_pixel_color[color_index.green] = (alpha_val * pixel_color[color_index.green] + (255 - alpha_val) * new_pixel_color [color_index.green]) / 255;
        new_pixel_color[color_index.blue] = (alpha_val * pixel_color[color_index.blue] + (255 - alpha_val) * new_pixel_color[color_index.blue]) / 255;
    }
    else
    {
        new_pixel_color = pixel_color;
    }
    *pixel_ptr = *(uint32_t*)new_pixel_color;
}

void	ei_fill			(ei_surface_t		surface,
                            const ei_color_t*	color,
                            const ei_rect_t*	clipper)
{
    if (surface == NULL) return;

    ei_point_t top_left;
    ei_size_t size;
    ei_size_t surface_size = hw_surface_get_size(surface);

    if (clipper != NULL) {
        top_left = clipper -> top_left;
        size = clipper -> size;
    } else {
        top_left = (ei_point_t){0, 0};
        size = surface_size;
    }

    uint8_t pixel_color[4];
    RGBAIndex color_index = assign_color(surface, color, &pixel_color);
    uint8_t alpha = color -> alpha;

    for (int y = top_left.y; y < top_left.y + size.height; y++)
    {
        for (int x = top_left.x; x < top_left.x + size.width; x++)
        {
          	ei_draw_pixel(x, y, surface_size.width, color_index, (uint32_t*)hw_surface_get_buffer(surface), pixel_color, alpha);
        }
    }
}

/**
 * \brief	Calculates the x and y coordinates of the next iteration of the Bresenham when the slope is greater than 1 in absolute value
 *
 * @param	params  	    Pointer to an ei_Bresenham_params structure, containing all the information to execute the Bresenham algorithm.
 */
static void update_draw_line_values_x(ei_Bresenham_params *params) {
    *params->x += params->sign_x;
    *params->E += params->mod_y;
    if (2**params->E > params->mod_x)
    {
        *params->y += params->sign_y;
        *params->E -= params->mod_x;
    }
}

static void ei_draw_line_x(ei_point_t point, ei_Bresenham_params *params) {
    bool continue_cond = *params->x <= point.x;
    while (continue_cond ? *params->x <= point.x : *params->x > point.x) {
        if (params->clipper != NULL)
        {
            if (params->clipper->top_left.x <= *params->x && params->clipper->top_left.y <= *params->y && *params->x <= params->clipper->top_left.x + params->clipper->size.width && *params->y <= params->clipper->top_left.y + params->clipper->size.height) {
                ei_draw_pixel(*params->x,*params->y, params->surface_size.width, params->color_index, params->surface_buffer, params->pixel_color, params->alpha);
            }
            update_draw_line_values_x(params);
        }
        else
        {
            ei_draw_pixel(*params->x,*params->y, params->surface_size.width, params->color_index, params->surface_buffer, params->pixel_color, params->alpha);
            update_draw_line_values_x(params);
        }
    }
}

/**
 * \brief	Calculates the x and y coordinates of the next iteration of the Bresenham when the slope is smaller than 1 in absolute value
 *
 * @param	params  	    Pointer to an ei_Bresenham_params structure, containing all the information to execute the Bresenham algorithm.
 */
static void update_draw_line_values_y(ei_Bresenham_params *params) {
    *params->y += params->sign_y;
    *params->E += params->mod_x;
    if (2**params->E > params->mod_y)
    {
        *params->x += params->sign_x;
        *params->E -= params->mod_y;
    }
}

/* The current direction (y <= point2.y) with the initial direction (continue_cond)
 * keeps looping as long as they're still the same
 * draw lines of y
 */
static void ei_draw_line_y(ei_point_t point, ei_Bresenham_params *params) {
    bool continue_cond = *params->y <= point.y;
    while (continue_cond ? *params->y <= point.y : *params->y > point.y)
    {
        if (params->clipper != NULL)
        {
            if (params->clipper->top_left.x <= *params->x && params->clipper->top_left.y <= *params->y && *params->x <= params->clipper->top_left.x + params->clipper->size.width && *params->y <= params->clipper->top_left.y + params->clipper->size.height) {
                ei_draw_pixel(*params->x, *params->y, params->surface_size.width, params->color_index, params->surface_buffer, params->pixel_color, params->alpha);
            }
            update_draw_line_values_y(params);
        }
        else
        {
            ei_draw_pixel(*params->x, *params->y, params->surface_size.width, params->color_index, params->surface_buffer, params->pixel_color, params->alpha);
            update_draw_line_values_y(params);
        }
    }
}

/**
 * \brief	Initializes the paramaterers to use the Bresenham algorithm, and then executes the algorithm calling
 *          \ref ei_draw_line_x or ref ei_draw_line_y
 *
 * @param	surface         The surface where the line will be drawn.
 *
 * @param	point1          The starting point of the line.
 *
 * @param	point2          The ending point of the line.
 *
 * @param	pixel_color     An array representing the RGBA color values to be used for the line.
 *
 * @param	mod_x           Absolute value of de difference of the x coordinate between point1 and point2.
 *
 * @param	mod_y           Absolute value of de difference of the x coordinate between point1 and point2.
 *
 * @param	surface_size    The size of the surface where the line will be drawn.
 *
 * @param	sign_x          The sign of the difference of the x-coordinate between point1 and point2.
 *
 * @param	sign_y          The sign of the difference of the y-coordinate between point1 and point2.
 *
 * @param	reverse         A flag indicating whether to reverse the x and y directions in the algorithm.
 *
 * @param	color_index     The RGBA index structure containing the color channel indices for the surface.
 *
 * @param	alpha           The alpha (transparency) value of the color.
 *
 * @param	clipper         A pointer to a clipping rectangle that confines the drawing region.
 *
 */

void Bresenham(ei_surface_t surface, ei_point_t point1, ei_point_t point2, uint8_t pixel_color[4], int mod_x, int mod_y, ei_size_t surface_size, int sign_x, int sign_y, bool reverse, RGBAIndex color_index, uint8_t alpha,const ei_rect_t* clipper) {

    int E = 0, x = point1.x, y = point1.y;
    uint32_t* surface_buffer = (uint32_t*)hw_surface_get_buffer(surface);

    // The parameters to be passed into the functions of draw line x or y
    ei_Bresenham_params params = {
        mod_x, mod_y,
        sign_x, sign_y,
        &E, &x, &y,
        surface_size,
        color_index,
        pixel_color,
        surface_buffer,
        clipper,
        alpha
    };
    /* if |delta_x|>|delta_y| then apply algo given in the pdf */
    /* if |delta_y|>|delta_x|, then inverse x and y in algo */
    if (!reverse) ei_draw_line_x(point2, &params);
    else ei_draw_line_y(point2, &params);
}


void ei_draw_line(ei_surface_t	surface, ei_point_t point1, ei_point_t point2, uint8_t pixel_color[4], RGBAIndex color_index, uint8_t alpha, const ei_rect_t* clipper)
{
    int delta_x = point2.x - point1.x;
    int delta_y = point2.y - point1.y;
    ei_size_t surface_size = hw_surface_get_size(surface);

    int sign_x = 1, sign_y = 1;
    if (delta_x < 0) sign_x = -1;
    if (delta_y < 0) sign_y = -1;
    const int mod_x = delta_x * sign_x;
    const int mod_y = delta_y * sign_y;
    bool reverse = false;
    if (mod_y > mod_x) reverse = true;
    Bresenham(surface, point1, point2, pixel_color, mod_x, mod_y, surface_size, sign_x, sign_y, reverse, color_index, alpha, clipper);
}

void	ei_draw_polyline(ei_surface_t		surface,
                     ei_point_t*		point_array,
                     size_t			point_array_size,
                     ei_color_t		color,
                     const ei_rect_t*	clipper)
{
    uint8_t pixel_color[4];
    RGBAIndex color_index = assign_color(surface, &color,  &pixel_color);
    uint8_t alpha = color.alpha;
    for (int i=1; i < point_array_size; i++)
    {
        ei_draw_line(surface, point_array[i-1], point_array[i], pixel_color, color_index, alpha, clipper);
    }
}


static void ei_polygon_fill_color(ei_surface_t surface, uint8_t pixel_color[4], int j, tab_poly *TCA, RGBAIndex color_index, uint8_t alpha, ei_point_t offset) {
    // Fill color in polygon
    tab_poly* current_cell=TCA;
    while (current_cell != NULL)
    {
        ei_point_t point2;
        ei_point_t point1;

        // Replacement condition : We round to the superior int
        if (current_cell->x != trunc(current_cell->x)) point1.x = (int) current_cell->x + 1;
        else point1.x = (int) current_cell->x;
        point1.y = j;
        current_cell = current_cell->next;

        // Replacement condition : We round to the inferior int
        if (current_cell->x == trunc(current_cell->x)) point2.x = (int) current_cell->x - 1;
        else point2.x = (int) current_cell->x;
        point2.y = j;

        point1.x -= offset.x;
        point1.y -= offset.y;
        point2.x -= offset.x;
        point2.y -= offset.y;

        if (point2.x > point1.x)
        {
            ei_draw_line(surface,point1,point2,pixel_color, color_index, alpha, NULL);
        }
        current_cell = current_cell->next;
    }
}


void	ei_draw_polygon		(ei_surface_t		surface,
                                ei_point_t*		point_array,
                                size_t			point_array_size,
                                ei_color_t		color,
                                const ei_rect_t*	clipper)
{
    if (surface == NULL) return;
    ei_surface_t drawing_surface = surface;
    ei_surface_t off_screen_surface = NULL;
    ei_rect_t off_screen_rect, surface_rect, copy_rect;
    surface_rect.top_left.x = 0;
    surface_rect.top_left.y = 0;
    uint8_t pixel_color[4];
    int size_y = hw_surface_get_size(surface).height;
    tab_poly** TC = calloc(1, size_y*sizeof(tab_poly*));
    int y_max_glob, x_max_glob, x_min_glob;
    int j = initialise_TC(point_array, point_array_size, TC, &y_max_glob, &x_max_glob, &x_min_glob);
    ei_point_t offset = {0,0};

    if (clipper != NULL)
    {
        surface_rect.top_left.x = x_min_glob;
        surface_rect.top_left.y = j;
        surface_rect.size.width = x_max_glob - x_min_glob;
        surface_rect.size.height = y_max_glob - j;

        off_screen_rect = surface_rect;
        off_screen_rect.top_left.x = 0;
        off_screen_rect.top_left.y = 0;
        off_screen_surface = hw_surface_create(surface, off_screen_rect.size, true);

        hw_surface_lock(off_screen_surface);
        copy_rect = intersection_clipper(surface_rect, &off_screen_rect, clipper);
        drawing_surface = off_screen_surface;

        offset.x = x_min_glob;
        offset.y = j;
    }
    RGBAIndex color_index = assign_color(drawing_surface, &color,  &pixel_color);
    tab_poly* TCA = NULL;
    do {
        // if TCA NULL, take the list of current y
        // else add the list to the end of the TCA
        // tab_poly* prev = TCA;
        tab_poly* curr = TCA;
        if (TCA == NULL)
        {
            TCA = TC[j];
        }
        else
        {
            while (curr -> next != NULL)
                curr = curr -> next;
            curr->next = TC[j];
        }

        // Delete points y_max==y
        tab_poly* prev = NULL;
        curr = TCA;
        while (curr != NULL)
        {
            if (curr->y == j)
            {
                delete_segment(&TCA, &prev, &curr);
            }
            else
            {
                prev = curr;
                curr = curr -> next;
            }
        }
        sort_tca(&TCA);
        uint8_t alpha_val = color.alpha;
        ei_polygon_fill_color(drawing_surface, pixel_color, j, TCA, color_index, alpha_val, offset);
        j++;

        // Update x_ymin of each cell
        curr = TCA;
        update_xymin(&curr);
    } while (TCA != NULL || TC[j] != NULL);

    if (clipper != NULL)
    {
        ei_copy_surface(surface,&copy_rect,off_screen_surface,&off_screen_rect, true);
        hw_surface_unlock(off_screen_surface);
        hw_surface_free(off_screen_surface);
    }
}

int initialized_arc(ei_point_t *point_array, ei_point_t center, int radius, int start_angle, int end_angle)
{
    int i, x, y, size = end_angle - start_angle;
    if (size < 0) size = -size;
    double conv_rad_to_grad = 2*M_PI/360;
    for (i=0; 10*i<=size; i++)
    {
        x = (int)(cos(conv_rad_to_grad * start_angle) * radius);
        y = (int)(sin(conv_rad_to_grad * start_angle) * radius);
        ei_point_t point = { x + center.x, y + center.y };
        point_array[i] = point;
        start_angle = start_angle + 10;
    }
    if (start_angle-10 < end_angle)
    {
        x = (int)(cos(conv_rad_to_grad * end_angle) * radius);
        y = (int)(sin(conv_rad_to_grad * end_angle) * radius);
        ei_point_t point = {x + center.x, y + center.y};
        point_array[i] = point;
        i++;
    }
    return i;
}

/**
 * \brief	Generates the coordinates for the top portion of a rounded rectangle, including arcs for the rounded corners.
 *
 * @param	rectangle               The rectangle representing the area for the top rounded arc.
 *
 * @param	radius                  The radius of the rounded corners.
 *
 * @param	size                    The size of the array that will hold the resulting points.
 *
 * @param	h                       Minimun of the height or width divided by 2.
 *
 * @param	rounded_rectangle_points An array of points that will store the coordinates of the rounded rectangle's corners and edges.
 *
 * @param	*next_segment_start      A pointer to the current point where the next point of the polygon should begin.
 *
 */

static void top_rounded_arc(const ei_rect_t rectangle, const int radius, int size, int h, ei_point_t* rounded_rectangle_points, ei_point_t** next_segment_start)
{
    ei_point_t end_point, arc_center;
    int next_segment_start_idx;

    // Bottom left arc
    arc_center.x = rectangle.top_left.x + radius;
    arc_center.y = rectangle.top_left.y + rectangle.size.height - radius;

    next_segment_start_idx = initialized_arc(*next_segment_start, arc_center, radius, 135, 180);

    ei_point_t half_end_point;

    half_end_point.x = rectangle.top_left.x + rectangle.size.width - h + 1;
    half_end_point.y = rectangle.top_left.y + h;
    rounded_rectangle_points[size-3] = half_end_point;

    half_end_point.x = rectangle.top_left.x + h + 1;
    half_end_point.y = rectangle.top_left.y + rectangle.size.height - h;
    rounded_rectangle_points[size-2] = half_end_point;

    rounded_rectangle_points[size-1] = rounded_rectangle_points[0];

    // Left segment
    *next_segment_start = *next_segment_start + next_segment_start_idx;

    end_point.x = arc_center.x - radius;
    end_point.y = arc_center.y - rectangle.size.height + 2 * radius;

    if (radius > rectangle.size.width / 2 || radius > rectangle.size.height / 2)
    {
        (*next_segment_start)[0] = rounded_rectangle_points[next_segment_start_idx-1];
    }
    else (*next_segment_start)[0] = end_point;

    // Top left arc
    *next_segment_start = *next_segment_start + 1;
    arc_center.x = end_point.x + radius;
    arc_center.y = end_point.y;
    next_segment_start_idx = initialized_arc(*next_segment_start, arc_center, radius, 180, 270);

    // Top segment
    *next_segment_start = *next_segment_start + next_segment_start_idx;
    end_point.x = arc_center.x + rectangle.size.width - 2 * radius;
    end_point.y = arc_center.y - radius;

    if (radius > rectangle.size.width / 2 || radius > rectangle.size.height / 2)
    {
        (*next_segment_start)[0] = rounded_rectangle_points[next_segment_start_idx-1];
    }
    else (*next_segment_start)[0] = end_point;

    // Top right arc
    *next_segment_start = *next_segment_start + 1;
    arc_center.x = end_point.x;
    arc_center.y = end_point.y + radius;
    next_segment_start_idx = initialized_arc(*next_segment_start, arc_center, radius, 270, 315);
}

/**
 * \brief	Generates the coordinates for the bottom portion of a rounded rectangle, including arcs for the rounded corners.
 *
 * @param	rectangle               The rectangle representing the area for the top rounded arc.
 *
 * @param	radius                  The radius of the rounded corners.
 *
 * @param	size                    The size of the array that will hold the resulting points.
 *
 * @param	h                       Minimun of the height or width divided by 2.
 *
 * @param	rounded_rectangle_points An array of points that will store the coordinates of the rounded rectangle's corners and edges.
 *
 * @param	*next_segment_start      A pointer to the current point where the next point of the polygon should begin.
 *
 */

static void bottom_rounded_frame(const ei_rect_t rectangle, const int radius, int size, int h, ei_point_t* rounded_rectangle_points, ei_point_t** next_segment_start)
{
    ei_point_t end_point, arc_center;

    // Top right arc
    arc_center.x = rectangle.top_left.x + rectangle.size.width - radius;
    arc_center.y = rectangle.top_left.y + radius;
    int next_segment_start_idx = initialized_arc(*next_segment_start, arc_center, radius, 315, 360);

    ei_point_t half_end_point;
    rounded_rectangle_points[size-1] = rounded_rectangle_points[0];

    half_end_point.x = rectangle.top_left.x + h;
    half_end_point.y = rectangle.top_left.y + rectangle.size.height - h;
    rounded_rectangle_points[size-3] = half_end_point;

    half_end_point.x = rectangle.top_left.x + rectangle.size.width - h;
    half_end_point.y = rectangle.top_left.y + h;
    rounded_rectangle_points[size-2] = half_end_point;
    rounded_rectangle_points[size-1] = rounded_rectangle_points[0];

    // Right segment
    *next_segment_start = *next_segment_start + next_segment_start_idx;
    end_point.x = arc_center.x + radius;
    end_point.y = arc_center.y + rectangle.size.height - 2*radius;

    if (radius > rectangle.size.width / 2 || radius > rectangle.size.height / 2)
    {
        (*next_segment_start)[0] = rounded_rectangle_points[next_segment_start_idx-1];
    }
    else (*next_segment_start)[0] = end_point;

    // Bottom right arc
    *next_segment_start = *next_segment_start + 1;
    arc_center.x = end_point.x - radius;
    arc_center.y = end_point.y;
    next_segment_start_idx = initialized_arc(*next_segment_start, arc_center, radius, 0, 90);

    // Bottom segment
    *next_segment_start = *next_segment_start + next_segment_start_idx;
    end_point.x = arc_center.x - rectangle.size.width + 2 * radius;
    end_point.y = arc_center.y + radius;

    if (radius > rectangle.size.width / 2 || radius > rectangle.size.height / 2)
    {
        (*next_segment_start)[0] = rounded_rectangle_points[next_segment_start_idx-1];
    }
    else (*next_segment_start)[0] = end_point;

    // Bottom left arc
    *next_segment_start = *next_segment_start + 1;
    arc_center.x = end_point.x;
    arc_center.y = end_point.y - radius;
    initialized_arc(*next_segment_start, arc_center, radius, 90, 135);
}

/**
 * \brief	Creates a point array of the coordinates of a rectangle with rounded corners
 *
 * @param	rectangle  	The rectangle to draw.
 *
 * @param	radius 		The radius of the corners
 *
 * @param	*array_points_size Pointer that will indicate the size of the final array. It initial pointed value is not important as it is overwritten during
 * 				   the execution of the funciton.
 * @param	top		If it is true in generates the coordinates of the top size of the button.
 *
 * @param	bottom		If it is true in generates the coordinates of the top size of the top.
 */
ei_point_t* rounded_frame(const ei_rect_t rectangle, const int radius, int *array_points_size, bool top, bool bottom)
{
    // Initialization
    int size;
    if (top && bottom) size = 48; //(24 * 2)
    else size = 27;

    int h = rectangle.size.height / 2;
    // width smaller than height
    if (rectangle.size.width < 2*h)
    {
        h = rectangle.size.width / 2;
    }

    ei_point_t* rounded_rectangle_points = malloc(sizeof(ei_point_t) * size);
    ei_point_t* next_segment_start = rounded_rectangle_points;

    if (top)
    {
        top_rounded_arc(rectangle, radius, size, h, rounded_rectangle_points, &next_segment_start);
        next_segment_start = rounded_rectangle_points + 24;
    }

    if (bottom)
    {
        bottom_rounded_frame(rectangle, radius, size, h, rounded_rectangle_points, &next_segment_start);
    }

    *array_points_size = size;
    return rounded_rectangle_points;
}

//Description en ei_button.h
void draw_button(const ei_surface_t surface, const ei_rect_t rectangle, const int radius, const int border_width, ei_rect_t* clipper, const ei_color_t* color, ei_relief_t relief)
{
	ei_color_t color_top;
	ei_color_t color_inside;
	ei_color_t color_bottom;

    if (color==NULL)
    {
        color_inside = ei_default_background_color; //Default value
    }
    else color_inside = *color;

    if (relief == ei_relief_none)
    {
        color_top = color_inside;
        color_bottom = color_inside;
    }
    else
    {
        color_top.alpha = color_inside.alpha;
        color_top.red = color_inside.red+30 > 255 ? 255 : color_inside.red+30;
        color_top.blue = color_inside.blue+30 > 255 ? 255 : color_inside.blue+30;
        color_top.green = color_inside.green+30 > 255 ? 255 : color_inside.green+30;

        color_bottom.alpha = color_inside.alpha;
        color_bottom.red = color_inside.red-30 < 0 ? 0 : color_inside.red-30;
        color_bottom.blue = color_inside.blue-30 < 0 ? 0 : color_inside.blue-30;
        color_bottom.green = color_inside.green-30 < 0 ? 0 : color_inside.green-30;

        if (relief == ei_relief_sunken)
        {
            const ei_color_t temp = color_top;
            color_top = color_bottom;
            color_bottom = temp;
        }
    }
  	int array_top_size = 0;
	int array_bottom_size = 0;
	int array_inside_size = 0;
  	ei_point_t* top_button = rounded_frame(rectangle, radius, &array_top_size, true, false);
  	ei_point_t* bottom_button = rounded_frame(rectangle, radius, &array_bottom_size, false, true);
	ei_rect_t inside_rectangle;
    inside_rectangle.size.width = rectangle.size.width - 2 * border_width;
    inside_rectangle.size.height = rectangle.size.height - 2 * border_width;
    inside_rectangle.top_left.x = rectangle.top_left.x + border_width;
    inside_rectangle.top_left.y = rectangle.top_left.y + border_width;
    int inside_radius = radius;
    if (radius>inside_rectangle.size.width / 2 || radius>inside_rectangle.size.height / 2)
    {
        inside_radius = inside_rectangle.size.height / 2;
        if (inside_rectangle.size.width<inside_rectangle.size.height) inside_radius = inside_rectangle.size.width / 2;
    }
	ei_point_t* inside_button = rounded_frame(inside_rectangle, inside_radius, &array_inside_size, true, true);

	ei_draw_polygon(surface, top_button, array_top_size, color_top, clipper);
	ei_draw_polygon(surface, bottom_button, array_bottom_size, color_bottom, clipper);
	if (relief != ei_relief_none)
		ei_draw_polygon(surface, inside_button, array_inside_size, color_inside, clipper);
    free(top_button);
    free(bottom_button);
    free(inside_button);
}

int	ei_copy_surface		(const ei_surface_t		destination,
                 const ei_rect_t*	dst_rect,
                 const ei_surface_t		source,
                 const ei_rect_t*	src_rect,
                 bool	alpha)
{
    ei_size_t source_surface_size = hw_surface_get_size(source);
    ei_size_t destination_surface_size = hw_surface_get_size(destination);

    if (dst_rect->size.width != src_rect->size.width || dst_rect->size.height != src_rect->size.height) return 1;

    uint32_t dst_x = dst_rect->top_left.x, dst_y = dst_rect->top_left.y;
    hw_surface_lock(source);
    hw_surface_lock(destination);
    uint32_t* destination_surface_buffer = (uint32_t*)hw_surface_get_buffer(destination);
    uint32_t* pixel_source_ptr = (uint32_t*)hw_surface_get_buffer(source) + src_rect->top_left.y * source_surface_size.width + src_rect->top_left.x;
    hw_surface_unlock(source);
    hw_surface_unlock(destination);

    int ir, ig, ib, ia;
    hw_surface_get_channel_indices(source, &ir, &ig, &ib, &ia);
    RGBAIndex color_index = { .red = ir, .green = ig, .blue = ib, .alpha = ia };

    if (alpha == true)
    {
        for (int i = 0; i < src_rect->size.height; i++)
        {
            for (int j = 0; j < src_rect->size.width; j++)
            {
                uint8_t* pixel_source_rgb = (uint8_t*)pixel_source_ptr;
                uint8_t alpha_val = pixel_source_rgb[ia];
                ei_draw_pixel((int32_t)dst_x, (int32_t)dst_y, destination_surface_size.width, color_index, destination_surface_buffer, pixel_source_rgb , alpha_val);
                pixel_source_ptr++;
                dst_x++;
            }
            pixel_source_ptr = pixel_source_ptr - src_rect->size.width + source_surface_size.width;
            dst_x = dst_rect->top_left.x;
            dst_y++;
        }
    }

    if (alpha == false)
    {
        for (int i = 0; i < src_rect->size.height; i++)
        {
            for (int j = 0; j < src_rect->size.width; j++)
            {
                uint8_t* pixel_source_rgb = (uint8_t*)pixel_source_ptr;
                ei_draw_pixel((int32_t)dst_x, (int32_t)dst_y, destination_surface_size.width, color_index, destination_surface_buffer, pixel_source_rgb , 255);
                pixel_source_ptr++;
                dst_x++;
            }
            pixel_source_ptr = pixel_source_ptr - src_rect->size.width + source_surface_size.width;
            dst_x = dst_rect->top_left.x;
            dst_y++;
        }

    }
    return 0;
}

void	ei_draw_text		(ei_surface_t		surface,
                 const ei_point_t*	where,
                 ei_const_string_t	text,
                 ei_font_t		font,
                 ei_color_t		color,
                 const ei_rect_t*	clipper)
{

    if (font == NULL) font = ei_default_font;
    if (text != NULL && strlen(text) == 0) return;

    ei_surface_t text_surface = hw_text_create_surface(text, font, color);
    ei_rect_t text_rect = hw_surface_get_rect(text_surface);
    ei_rect_t surface_rect = {*where, {text_rect.size.width, text_rect.size.height}};

    //If clipper isn't null we change the destination rectangle
    if (clipper != NULL) {
        surface_rect = intersection_clipper(surface_rect, &text_rect, clipper);
    }
    ei_copy_surface(surface, &surface_rect, text_surface, &text_rect, true);
    hw_surface_free(text_surface);
}

//Description en ei_frame.h
ei_rect_t intersection_clipper(ei_rect_t surface_rect, ei_rect_t* text_rect, const ei_rect_t* clipper)
{
    if (surface_rect.top_left.x < clipper->top_left.x) {
        if (text_rect != NULL)
        {
            text_rect->top_left.x = text_rect->top_left.x + (clipper->top_left.x - surface_rect.top_left.x);
            text_rect->size.width -= clipper->top_left.x - surface_rect.top_left.x;
        }
        surface_rect.size.width -= clipper->top_left.x - surface_rect.top_left.x;
        surface_rect.top_left.x = clipper->top_left.x;
    }

    if (surface_rect.top_left.y < clipper->top_left.y) {
        if (text_rect != NULL)
        {
            text_rect->top_left.y = text_rect->top_left.y + (clipper->top_left.y - surface_rect.top_left.y);
            text_rect->size.height -= clipper->top_left.y - surface_rect.top_left.y;
        }
        surface_rect.size.height -= clipper->top_left.y - surface_rect.top_left.y;
        surface_rect.top_left.y = clipper->top_left.y;
    }

    if (surface_rect.top_left.x + surface_rect.size.width > clipper->top_left.x + clipper->size.width) {
        surface_rect.size.width = clipper->top_left.x + clipper->size.width - surface_rect.top_left.x;
        if (text_rect != NULL) {
            text_rect->size.width = clipper->top_left.x + clipper->size.width - surface_rect.top_left.x;
        }
    }

    if (surface_rect.top_left.y + surface_rect.size.height > clipper->top_left.y + clipper->size.height) {
        surface_rect.size.height = clipper->top_left.y + clipper->size.height - surface_rect.top_left.y;
        if (text_rect != NULL) {
            text_rect->size.height = clipper->top_left.y + clipper->size.height - surface_rect.top_left.y;
        }
    }
    return surface_rect;
}

//Description in ei_toplevel.h
ei_point_t* rounded_top_corners(const ei_rect_t rectangle, const int radius, int *array_points_size)
{
    // Size needed for top-left and top-right arcs plus segments
    int size = 25;
    *array_points_size = size;
    int next_segment_start_idx = 0;
    ei_point_t* rounded_rectangle_points = malloc(sizeof(ei_point_t) * size);
    ei_point_t* next_segment_start = rounded_rectangle_points;

    // Top left arc
    ei_point_t arc_center = {rectangle.top_left.x + radius, rectangle.top_left.y + radius};
    next_segment_start_idx = initialized_arc(next_segment_start, arc_center, radius, 180, 270); // + 10

    // Top segment
    next_segment_start = next_segment_start + next_segment_start_idx;
    ei_point_t end_point = {rectangle.top_left.x + rectangle.size.width - radius, arc_center.y - radius};
    next_segment_start[0] = end_point; // + 1

    // Top right arc
    next_segment_start = next_segment_start + 1;
    arc_center.x = end_point.x;
    arc_center.y = end_point.y + radius;
    next_segment_start_idx = initialized_arc(next_segment_start, arc_center, radius, 270, 360); // + 10

    // Right segment
    next_segment_start = next_segment_start + next_segment_start_idx;
    end_point.x = arc_center.x + radius;
    end_point.y = rectangle.top_left.y + rectangle.size.height;
    next_segment_start[0] = end_point; // + 1

    // Bottom segment
    next_segment_start = next_segment_start + 1;
    end_point.x = rectangle.top_left.x;
    next_segment_start[0] = end_point; // + 1

    // Left segment
    next_segment_start = next_segment_start + 1;
    end_point.y = rectangle.top_left.y + radius;
    next_segment_start[0] = end_point; // + 1

    // Close rect
    next_segment_start = next_segment_start + 1;
    next_segment_start[0] = rounded_rectangle_points[0];

    return rounded_rectangle_points;
}