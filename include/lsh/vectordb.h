/**
 * @file vectordb.h
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 * @date 2013
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
 * @brief Declaration of structures and functions on list databases
 */
#ifndef VECTORDB_H
#define VECTORDB_H

#include "vectors.h"

typedef struct VectorDB{
     uint size;
     uint dim;
     Vector *vectors;
}VectorDB;

/************************ Function prototypes ************************/
void vectordb_init(VectorDB *);
VectorDB vectordb_create(uint, uint);
void vectordb_clear(VectorDB *);
void vectordb_destroy(VectorDB *);
void vectordb_print(VectorDB *);
void vectordb_print_multi(VectorDB *, List *);
void vectordb_print_range(VectorDB *, uint, uint);
void vectordb_apply_to_all(VectorDB *, void (*)(Vector *));
void vectordb_apply_to_range(VectorDB *, void (*)(Vector *), uint, uint);
void vectordb_apply_to_multi(VectorDB *, void (*)(Vector *), List *);
void vectordb_push(VectorDB *, Vector *);
void vectordb_pop(VectorDB *);
void vectordb_pop_multi(VectorDB *, uint);
void vectordb_pop_until(VectorDB *, uint);
void vectordb_delete_position(VectorDB *, uint);
void vectordb_delete_range(VectorDB *, uint, uint);
void vectordb_delete_smallest(VectorDB *, uint);
void vectordb_delete_largest(VectorDB *, uint);
void vectordb_insert(VectorDB *, Vector *, uint);
void vectordb_append(VectorDB *, VectorDB *);
VectorDB vectordb_load_from_file(char *);
void vectordb_save_to_file(char *, VectorDB *);
#endif
