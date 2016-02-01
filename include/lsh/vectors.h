/**
 * @file vectors.h
 * @author Gibran Fuentes-Pineda <gibranfp@unam.mx>
 * @date 2016
 *
 * @section GPL
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @brief Declaration of structures and functions on array vectors
 */
#ifndef VECTORS_H
#define VECTORS_H

#include "array_lists.h"

typedef struct Dim {
     uint dim;
     double value;
} Dim;


typedef struct Vector {
     uint size;
     Dim *data;
} Vector;

/************************ Function prototypes ************************/
void vector_init(Vector *);
Vector vector_create(uint);
void vector_destroy(Vector *);
Dim *vector_min_value(Vector *);
Dim *vector_max_value(Vector *);
uint vector_sum_value(Vector *);
void vector_print(Vector *);
void vector_print_multi(Vector *, List *);
void vector_print_range(Vector *, uint, uint);
void vector_push(Vector *, Dim);
void vector_pop(Vector *);
Vector vector_duplicate(Vector *);
#endif
