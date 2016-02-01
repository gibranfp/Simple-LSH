/**
 * @file l1lsh.h
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
#ifndef L1LSH_H
#define L1LSH_H

#include "types.h"
#include "listdb.h"


typedef struct {
     unsigned int loc;
     unsigned int dim;
} SampleBits;

typedef struct BucketL1 {
     ullong hash_value;
     List items;
} BucketL1;

typedef struct HashTableL1 {
     uint table_size; 
     uint tuple_size;
     uint max_value;
     uint dim;
     SampleBits *sample_bits;
     uint *number_of_samples;
     BucketL1 *buckets;
     List used_buckets;
     uint *a;
     uint *b;
} HashTableL1;

typedef struct HashIndexL1 {
	uint number_of_tables;
	HashTableL1 *hash_tables;
} HashIndexL1;

/************************ Function prototypes ************************/
void l1lsh_print_head(HashTableL1 *);
void l1lsh_print_table(HashTableL1 *);
void l1lsh_rng_init(unsigned long long);
void l1lsh_init(HashTableL1 *);
void l1lsh_generate_sample_bits(uint, uint, uint, SampleBits *, uint *);
HashTableL1 l1lsh_create(uint, uint, uint, uint);
void l1lsh_destroy(HashTableL1 *);
void l1lsh_erase_from_list(List *, HashTableL1 *);
void l1lsh_erase_from_index(uint, HashTableL1 *);
void l1lsh_clear_table(HashTableL1 *);
void l1lsh_destroy(HashTableL1 *);
void l1lsh_compute_hash_value(List *, HashTableL1 *, uint *, uint *);
uint l1lsh_get_index(List *, HashTableL1 *);
uint l1lsh_store_list(List *, uint, HashTableL1 *);
void l1lsh_store_listdb(ListDB *, HashTableL1 *, uint *);
int l1lsh_sample_bit_compare(const void *, const void *);
#endif
