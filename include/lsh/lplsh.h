/**
 * @file lplsh.h
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
 * @brief Declaration of structures and functions for Locality Sensitive Hashing
 */
#ifndef LPLSH_H
#define LPLSH_H

#include "types.h"
#include "listdb.h"
#include "vectordb.h"

typedef struct BucketLP {
     ullong hash_value;
     List items;
} BucketLP;

typedef struct HashTableLP {
     uint table_size; 
     uint tuple_size;
     uint dim;
     double *avec;
     double width;
     double *bval;
     BucketLP *buckets;
     List used_buckets;
     uint *a;
     uint *b;
} HashTableLP;

typedef struct HashIndexLP {
	uint number_of_tables;
	HashTableLP *hash_tables;
} HashIndexLP;

/************************ Function prototypes ************************/
double lplsh_rng_gaussian(void);
double lplsh_rng_cauchy(void);
double lplsh_rng_unif(double, double);
void lplsh_print_head(HashTableLP *);
void lplsh_print_table(HashTableLP *);
void lplsh_rng_init(unsigned long long);
void lplsh_init(HashTableLP *);
void lplsh_generate_random_values(uint, uint, double, double *, double *,
                                  double (*)(void));
HashTableLP lplsh_create(uint, uint, uint, double);
void lplsh_destroy(HashTableLP *);
void lplsh_erase_from_list(List *, HashTableLP *);
void lplsh_erase_from_index(uint, HashTableLP *);
void lplsh_clear_table(HashTableLP *);
void lplsh_destroy(HashTableLP *);
ullong lplsh_compute_hash_value(Vector *, double *, double, double);
void lplsh_univhash(Vector *, HashTableLP *, uint *, uint *);
uint lplsh_get_index(Vector *, HashTableLP *);
uint lplsh_store_vector(Vector *, uint, HashTableLP *);
void lplsh_store_vectordb(VectorDB *, HashTableLP *, uint *);
#endif
