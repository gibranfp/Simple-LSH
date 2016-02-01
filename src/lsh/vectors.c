/**
 * @file vectors.c
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 * @date 2015
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
 * @brief Basic operations on vectors.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <float.h>
#include "vectors.h"

/**
 * @brief Initializes a vector
 *
 * @param vector Vector to be initialized
 */
void vector_init(Vector *vector)
{
     vector->size = 0;
     vector->data = NULL;
}

/**
 * @brief Creates a vector of a given size initialized to zero
 *
 * @param size Size of the vector to be created
 *
 * @return Created vector
 */
Vector vector_create(uint size)
{
     Vector vector;

     vector.size = size;
     vector.data = (Dim *) calloc(size, sizeof(Dim));

     return vector;
}

/**
 * @brief Destroys a vector
 *
 * @param vector Vector to be destroyed
 */
void vector_destroy(Vector *vector)
{
     free(vector->data);
     vector_init(vector);
}

/**
 * @brief Finds the dim with the minimum value of an unordered vector
 *
 * @param vector Vector where the dim with the minimum value is to be searched
 *
 * @return Pointer to the dim with the minimum value
 */
Dim *vector_min_value(Vector *vector)
{
     uint i;
     Dim *min_value;

     min_value = vector->data;
     for (i = 1; i < vector->size; i++)
          if (min_value->value > vector->data[i].value)
               min_value = vector->data + i;

     return min_value;
}

/**
 * @brief Finds maximum value of an unordered vector
 *
 * @param vector Vector where the dim with the maximum value is to be searched
 *
 * @return Pointer to the dim with the maximum value
 */

Dim *vector_max_value(Vector *vector)
{
     uint i;
     Dim *max_value;

     max_value = vector->data;
     for (i = 1; i < vector->size; i++)
          if (max_value->value < vector->data[i].value)
               max_value = vector->data + i;

     return max_value;
}

/**
 * @brief Computes the sum of a vector
 *
 * @param vector Vector from which the sum is to be computed
 *
 * @return Sum of the input vector
 */
uint vector_sum_value(Vector *vector)
{
     uint i;
     uint sum = 0;

     for (i = 0; i < vector->size; i++)
          sum += vector->data[i].value;
              
     return sum;
}

/**
 * @brief Vector dim comparison for bsearch and qsort.
 *
 * @param a First dim to compare
 * @param b Second dim to compare
 *
 * @return 0 if the dims are equal, positive if the first dim
 *         is greater than the second and negative otherwise.
 */
int vector_dim_compare(const void *a, const void *b)
{
     int a_dim = ((Dim *)a)->dim;
     int b_dim = ((Dim *)b)->dim;

     return a_dim - b_dim;
}

/**
 * @brief Vector dim comparison for bsearch and qsort.
 *
 * @param a First dim to compare
 * @param b Second dim to compare
 *
 * @return 0 if the dims are equal, positive if the second dim
 *         is greater than the first and negative otherwise.
 */
int vector_dim_compare_back(const void *a, const void *b)
{
     int a_dim = ((Dim *)a)->dim;
     int b_dim = ((Dim *)b)->dim;

     return b_dim - a_dim;
}

/**
 * @brief Vector value comparison for bsearch and qsort.
 *
 * @param a First dim to compare
 * @param b Second dim to compare
 *
 * @return 0 if the dims are equal, positive if the value of first dim
 *         is greater than the second and negative otherwise.
 */
int vector_value_compare(const void *a, const void *b)
{
     int a_value = ((Dim *)a)->value;
     int b_value = ((Dim *)b)->value;

     return a_value - b_value;
}

/**
 * @brief Vector value comparison for bsearch qsort.
 *
 * @param a First dim to compare
 * @param b Second dim to compare
 *
 * @return 0 if the dims are equal, positive if the value of second dim
 *         is greater than the first and negative otherwise.
 */
int vector_value_compare_back(const void *a, const void *b)
{
     int a_value = ((Dim *)a)->value;
     int b_value = ((Dim *)b)->value;

     return b_value - a_value;
}

/**
 * @brief Score comparison for bsearch and qsort. 
 *
 * @param *a First score to compare
 * @param *b Second score to compare
 *
 * @return 0 if the scores are equal, positive if the first score
 *         is greater than the second and negative otherwise.
 */
int vector_score_compare(const void *a, const void *b)
{
     double a_val = ((Score *)a)->value;
     double b_val = ((Score *)b)->value;

     return a_val - b_val;
}

/**
 * @brief Score comparison for bsearch and qsort. 
 *
 * @param *a First score to compare
 * @param *b Second score to compare
 *
 * @return 0 if the scores are equal, positive if the second score
 *         is greater than the first and negative otherwise.
 */
int vector_score_compare_back(const void *a, const void *b)
{
     double a_val = ((Score *)a)->value;
     double b_val = ((Score *)b)->value;

     return b_val - a_val;
}

/**
 * @brief Searches for an dim in a sorted vector using the binary search algorithm.
 *
 * @param vector Vector where the dim is to be searched
 * @param dim Dim to be searched
 *
 * @return Pointer to the found dim
 */
Dim *vector_binary_search(Vector *vector, Dim query)
{
     return bsearch(&query, vector->data, vector->size, sizeof(Dim), vector_dim_compare);
}

/**
 * @brief Sorts a vector based on dim values in ascending order
 *
 * @param vector Vector to be sorted
 */
void vector_sort_by_dim(Vector *vector)
{
     qsort(vector->data, vector->size, sizeof(Dim), vector_dim_compare);
}

/**
 * @brief Sorts a vector based on dim values in descending order
 *
 * @param vector Vector to be sorted
 */
void vector_sort_by_dim_back(Vector *vector)
{
     qsort(vector->data, vector->size, sizeof(Dim), vector_dim_compare_back);
}

/**
 * @brief Sorts a vector based on dims value in ascending order
 *
 * @param vector Vector to be sorted
 */
void vector_sort_by_value(Vector *vector)
{
     qsort(vector->data, vector->size, sizeof(Dim), vector_value_compare);
}

/**
 * @brief Sorts a vector based on dims value in descending order
 *
 * @param vector Vector to be sorted
 */
void vector_sort_by_value_back(Vector *vector)
{
     qsort(vector->data, vector->size, sizeof(Dim), vector_value_compare_back);
}

/**
 * @brief Prints in screen the dims of a vector
 *
 * @param vector Vector to be printed
 */
void vector_print(Vector *vector)
{
     uint i;

     printf ("%d -- ", vector->size);
     for (i = 0; i < vector->size; i++)
          printf ("%d:%lf[%d] ", vector->data[i].dim, vector->data[i].value, i);
     printf("\n");
}

/**
 * @brief Prints in screen the dims of a vector specified by another vector
 *
 * @param vector Vector to be printed
 * @param positions Positions of the vector dims to be printed
 */
void vector_print_multi(Vector *vector, List *positions)
{
     uint i, pos;

     printf ("%d -- ", positions->size);
     for (i = 0; i < positions->size; i++) {
          pos = positions->data[i].item;
          if (pos >= 0 && pos < vector->size)
               printf ("%d:%lf[%d]", vector->data[pos].dim, vector->data[pos].value, pos);
          else
               printf ("%d-OOR ", pos);
     }
     printf ("\n");
}

/**
 * @brief Prints in screen the dims in a given vector range
 *
 * @param vector Vector to be printed
 * @param low Start position of the range
 * @param high End position of the range
 */
void vector_print_range(Vector *vector, uint low, uint high)
{
     uint i;

     if (low >= 0 && low <= high && high < vector->size) {
          printf ("%d (%d-%d) -- ", vector->size, low, high);
          for (i = low; i <= high; i++)
               printf ("%d:%lf[%d] ", vector->data[i].dim, vector->data[i].value, i);
     } else {
          printf ("Invalid range %d-%d. Vector range is 0-%d", low, high,
                  vector->size - 1);
     }
     printf ("\n");
}

/**
 * @brief Adds a new dim to the end of a vector
 *
 * @param vector Vector where the dim will be added
 * @param dim Dim to be added
 */
void vector_push(Vector *vector, Dim dim)
{
     uint newsize = vector->size + 1;
     vector->data = realloc(vector->data, newsize * sizeof(Dim));
     vector->data[vector->size] = dim;
     vector->size = newsize;
}

/**
 * @brief Removes the dim at the end of a vector
 *
 * @param vector Vector where the dim will be removed
 */
void vector_pop(Vector *vector)
{
     vector->size--;
     vector->data = realloc(vector->data, vector->size * sizeof(Dim));
}

/**
 * @brief Duplicates a vector
 *
 * @param vector Vector to be duplicated
 *
 * @return Duplicated vector
 */
Vector vector_duplicate(Vector *src)
{
     Vector duplicate;

     duplicate.data = (Dim *) malloc(src->size * sizeof(Dim));
     memcpy(duplicate.data, src->data, src->size * sizeof(Dim));
     duplicate.size = src->size;

     return duplicate;
}
