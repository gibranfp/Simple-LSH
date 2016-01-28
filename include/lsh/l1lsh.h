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

typedef struct Bucket {
     ullong hash_value;
     List items;
} Bucket;

typedef struct HashTable {
     uint table_size; 
     uint tuple_size;
     uint max_value;
     uint dim;
     SampleBits *sample_bits;
     uint *number_of_samples;
     Bucket *buckets;
     List used_buckets;
     uint *a;
     uint *b;
} HashTable;

typedef struct HashIndex {
	uint number_of_tables;
	HashTable *hash_tables;
} HashIndex;

/************************ Function prototypes ************************/
void l1lsh_print_head(HashTable *);
void l1lsh_print_table(HashTable *);
void l1lsh_rng_init(unsigned long long);
void l1lsh_init(HashTable *);
void l1lsh_generate_sample_bits(uint, uint, uint, SampleBits *, uint *);
HashTable l1lsh_create(uint, uint, uint, uint);
void l1lsh_destroy(HashTable *);
void l1lsh_erase_from_list(List *, HashTable *);
void l1lsh_erase_from_index(uint, HashTable *);
void l1lsh_clear_table(HashTable *);
void l1lsh_destroy(HashTable *);
void l1lsh_compute_hash_value(List *, HashTable *, uint *, uint *);
uint l1lsh_get_index(List *, HashTable *);
uint l1lsh_store_list(List *, uint, HashTable *);
void l1lsh_store_listdb(ListDB *, HashTable *, uint *);
int l1lsh_sample_bit_compare(const void *, const void *);
#endif
