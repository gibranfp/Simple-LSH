/**
 * @file sampledlsh.c
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
 * @brief Functions to perform Sampled Locality-Sensitive Hashing (SLSH)
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sampledlsh.h"

/**
 * @brief Retrieves the items that were 
 *
 * @param listdb Database of lists to be hashed
 * @param hash_table Hash table
 */ 
void sampledlsh_l1_get_coitems(ListDB *coitems, HashTableL1 *hash_table)
{
     uint i;

     for (i = 0; i < hash_table->used_buckets.size; i++){ // scan buckets to find co-occurring items
          listdb_push(coitems, &hash_table->buckets[hash_table->used_buckets.data[i].item].items);
          list_init(&hash_table->buckets[hash_table->used_buckets.data[i].item].items);
          hash_table->buckets[hash_table->used_buckets.data[i].item].hash_value = 0;
     }

     list_destroy(&hash_table->used_buckets);
}

/**
 * @brief Retrieves the items that were hashed
 *
 * @param coitems Co-occurring items
 * @param hash_table Hash table
 */ 
void sampledlsh_lp_get_coitems(ListDB *coitems, HashTableLP *hash_table)
{
     uint i;

     for (i = 0; i < hash_table->used_buckets.size; i++){ // scan buckets to find co-occurring items
          listdb_push(coitems, &hash_table->buckets[hash_table->used_buckets.data[i].item].items);
          list_init(&hash_table->buckets[hash_table->used_buckets.data[i].item].items);
          hash_table->buckets[hash_table->used_buckets.data[i].item].hash_value = 0;
     }

     list_destroy(&hash_table->used_buckets);
}


/**
 * @brief Function for mining a database of lists based on Locality-Sensitive Hashing without weighting.
 *
 * @param listdb Database of binary lists
 * @param tuple_size Number of Hash values per tuple
 * @param number_of_tuples Number of Hash tuples
 * @param table_size Number of buckets in the hash table
 */
ListDB sampledlsh_l1mine(ListDB *listdb, uint tuple_size, uint number_of_tuples, uint max_value, uint table_size)
{
     HashTableL1 hash_table = l1lsh_create(table_size, tuple_size, listdb->dim, max_value);
     uint *indices = (uint *) malloc(listdb->size * sizeof(uint));
     ListDB coitems;
     listdb_init(&coitems);
     coitems.dim = listdb->size;

     // Hashing database & storing candidates
     uint i;
     for (i = 0; i < number_of_tuples; i++){
          printf("Mining table %u/%u: %u sample_bits for %u lists\r",
                 i + 1, number_of_tuples, tuple_size, listdb->size);
          l1lsh_generate_sample_bits(listdb->dim, max_value, tuple_size, hash_table.sample_bits, hash_table.number_of_samples);
          l1lsh_store_listdb(listdb, &hash_table, indices);
          sampledlsh_l1_get_coitems(&coitems, &hash_table);
     }

     l1lsh_destroy(&hash_table);
     free(indices);


     return coitems;
}

/**
 * @brief Function for mining a database of lists based on Locality-Sensitive Hashing without weighting.
 *
 * @param listdb Database of binary lists
 * @param tuple_size Number of Hash values per tuple
 * @param number_of_tuples Number of Hash tuples
 * @param table_size Number of buckets in the hash table
 */
ListDB sampledlsh_lpmine(VectorDB *vectordb, uint tuple_size, uint number_of_tuples, double width, uint table_size, double (*ps_dist)(void))
{
     HashTableLP hash_table = lplsh_create(table_size, tuple_size, vectordb->dim, width);
     uint *indices = (uint *) malloc(vectordb->size * sizeof(uint));
     ListDB coitems;
     listdb_init(&coitems);
     coitems.dim = vectordb->size;

     // Hashing database & storing candidates
     uint i;
     for (i = 0; i < number_of_tuples; i++){
          printf("Mining table %u/%u: %u hash values for %u vectors\r",
                 i + 1, number_of_tuples, tuple_size, vectordb->size);
          lplsh_generate_random_values(tuple_size, vectordb->dim, width,
                                       hash_table.avec, hash_table.bval,
                                       ps_dist);
          lplsh_store_vectordb(vectordb, &hash_table, indices);
          sampledlsh_lp_get_coitems(&coitems, &hash_table);
     }

     lplsh_destroy(&hash_table);
     free(indices);


     return coitems;
}
