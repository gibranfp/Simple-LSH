/**
 * @file lplsh.c
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
 * @brief Functions for doing the locality sensitive hashing for l1
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mt64.h"
#include "lplsh.h"

/**
 * @Brief Generates normally distributed numbers using the Box-Muller transform
 *
 */
double lplsh_rng_gaussian(void)
{
     double u1, u2;

     do {
          u1 = genrand64_real3();
          u2 = genrand64_real3();
     } while (u1 <= DBL_MIN);
          

     return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

/**
 * @Brief Generates Cauchy distributed numbers from the ratio of 2 normally distributed numbers
 *        (ratio distribution).
 */
double lplsh_rng_cauchy(void)
{
     double a, b;

     a = lplsh_rng_gaussian();
     b = lplsh_rng_gaussian();

     if (fabs(b) < 0.0000001) {
          b = 0.0000001;
     }

     return a / b;
}

/**
 * @Brief Generates uniformly distributed real numbers.
 */
double lplsh_rng_unif(double start, double end)
{
     double u1 = genrand64_real3() * (end - start) + start;
}

/**
 * @Brief Prints head of a hash table structure
 *
 * @param hash_table Hash table structure
 */
void lplsh_print_head(HashTableLP *hash_table)
{
     uint i, j;

     printf("========== Hash table =========\n");
     printf("Table size: %d\n"
            "Sketch size: %d\n"
            "Width: %lf\n"
            "Dimensionality: %d\n"
            "Used buckets: ",
            hash_table->table_size, 
            hash_table->tuple_size,
            hash_table->width,
            hash_table->dim);

     printf("avec: [");
     for (i = 0; i < hash_table->tuple_size; i++){
          printf("avec: [");
          for (j = 0; j < hash_table->dim; j++)
               printf("%lf ", hash_table->avec[i * hash_table->dim + j]);
          printf("]\n");
     }
     printf("]\n");
     
     printf("bval: [");
     for (i = 0; i < hash_table->tuple_size; i++)
          printf("%lf ", hash_table->bval[i]);
     printf("]\n");
     
     list_print(&hash_table->used_buckets);

     printf("a: ");
     for (i = 0; i < hash_table->tuple_size; i++)
          printf("%u ", hash_table->a[i]);

     printf("\nb: ");
     for (i = 0; i < hash_table->tuple_size; i++)
          printf("%u ", hash_table->b[i]);
     printf("\n");
}


/**
 * @brief Prints head and content of a hash table structure
 *
 * @param hash_table Hash table structure
 */
void lplsh_print_table(HashTableLP *hash_table)
{
     uint i;
     
     for (i = 0; i < hash_table->used_buckets.size; i++){
          printf("[  %d  ] ", hash_table->used_buckets.data[i].item);
          list_print(&hash_table->buckets[hash_table->used_buckets.data[i].item].items);
     }
}

/**
 * @brief Initializes a hash table structure for performing LPLSH.
 *
 * @param hash_table Hash table structure
 */
void lplsh_init(HashTableLP *hash_table)
{
     hash_table->table_size = 0;
     hash_table->tuple_size = 0; 
     hash_table->dim = 0; 
     hash_table->avec  = NULL;
     hash_table->bval  = NULL;
     hash_table->width  = 0;
     hash_table->buckets = NULL;
     list_init(&hash_table->used_buckets);
     hash_table->a = NULL;
     hash_table->b = NULL;
}

/**
 * @brief Initializes the randon number generator
 */
void lplsh_rng_init(unsigned long long seed)
{
     init_genrand64(seed);
}


/**
 * @brief Creates a hash table structure for performing LSH.
 *
 * @param table_size Size of the hash table
 * @param tuple_size Number of hash functions per sketch
 * @param dim Dimensions of the vectors to be hashed
 *
 * @return Hash table structure
 */
HashTableLP lplsh_create(uint table_size, uint tuple_size, uint dim, double width)
{
     uint i;
     HashTableLP hash_table;

     hash_table.table_size = table_size;
     hash_table.tuple_size = tuple_size; 
     hash_table.dim = dim;
     hash_table.width = width;
     
     hash_table.avec = (double *) malloc(tuple_size * dim * sizeof(double));
     hash_table.bval = (double *) malloc(tuple_size *  sizeof(double));
     hash_table.buckets = (BucketLP *) calloc(table_size, sizeof(BucketLP));
     list_init(&hash_table.used_buckets);

     // generates array of random values for universal hashing
     hash_table.a = (uint *) malloc(tuple_size * sizeof(uint));
     hash_table.b = (uint *) malloc(tuple_size * sizeof(uint));
     for (i = 0; i < tuple_size; i++){
          hash_table.a[i] = (unsigned int) (genrand64_int64() & 0xFFFFFFFF);
          hash_table.b[i] = (unsigned int) (genrand64_int64() & 0xFFFFFFFF);
     }
     
     return hash_table;
}

/**
 * @brief Removes items stored in a bucket whose index is computed from a given vector
 *
 * @param vector Vector to be removed
 * @param hash_table Hash table structure
 */
void lplsh_erase_from_vector(Vector *vector, HashTableLP *hash_table)
{  
     uint index = lplsh_get_index(vector, hash_table);
     list_destroy(&hash_table->buckets[index].items);
     hash_table->buckets[index].hash_value = 0;
     
     Item item = {index, 1};
     Item *ptr = list_find(&hash_table->used_buckets, item);
     uint position = (uint) (ptr - hash_table->used_buckets.data);
     list_delete_position(&hash_table->used_buckets, position);
}

/**
 * @brief Removes items in a given bucket of the hash table
 *
 * @param index Index of the bucket to be removed
 * @param hash_table Hash table structure
 */
void lplsh_erase_from_index(uint index, HashTableLP *hash_table)
{  
     if (index >= 0 && index < hash_table->table_size){
          // destroy bucket
          list_destroy(&hash_table->buckets[index].items);
          hash_table->buckets[index].hash_value = 0;

          // delete bucket index from list of used buckets
          Item item = {index, 1};
          Item *ptr = list_find(&hash_table->used_buckets, item);
          uint position = (uint) (ptr - hash_table->used_buckets.data);
          list_delete_position(&hash_table->used_buckets, position);
     } else {
          printf("Index %u out of range! Table size is %u", index, hash_table->table_size);
     }
}

/**
 * @brief Removes the items in all the used buckets of the hash table
 *
 * @param hash_table Hash table structure
 */
void lplsh_clear_table(HashTableLP *hash_table)
{  
     uint i;

     // empties used buckets of a hash table
     for (i = 0; i < hash_table->used_buckets.size; i++) {
          list_destroy(&hash_table->buckets[hash_table->used_buckets.data[i].item].items);
          hash_table->buckets[hash_table->used_buckets.data[i].item].hash_value = 0;
     }

     list_destroy(&hash_table->used_buckets);
}

/**
 * @brief Destroys a hash table structure 
 *
 * @param hash_table Hash table structure
 */
void lplsh_destroy(HashTableLP *hash_table)
{
     free(hash_table->avec);
     free(hash_table->bval);
     free(hash_table->buckets);
     free(hash_table->a);
     free(hash_table->b);
     list_destroy(&hash_table->used_buckets);
     lplsh_init(hash_table);
}

/**
 * @brief Generates a random values for LSH scheme.
 *
  * @param tuple_size Number of hash values per tuple
 * @param dim Dimension of the input vectors
 * @param width Width parameter for computing hash value
 * @param avec Array of real numbers drawn from a p-stable distribution
 * @param bval Random value from U(0, width)
 */
void lplsh_generate_random_values(uint tuple_size, uint dim, double width,
                                  double *avec, double *bval,
                                  double (*ps_dist)(void))
{
     uint i, j;
     for (i = 0; i < tuple_size; i++){
          for (j = 0; j < dim; j++)
               avec[i * dim + j] = ps_dist();
          bval[i] = lplsh_rng_unif(0, width);
     }
}

/**
 * @brief Computes the hash value of a positive-integer-valued vector according to 
 * 
 * @param vector d-dimensional Euclidian vector
 * @param avec Array of real numbers drawn from a p-stable distribution
 * @param bval Value Real number drawn from U(0, width)
 * 
 * @return The hash value of input vector
 */ 
ullong lplsh_compute_hash_value(Vector *vector, double *avec, double bval, double width)
{
     uint i;

     double dotp = 0;
     for (i = 0; i < vector->size; i++) 
          dotp += vector->data[i].value * avec[vector->data[i].dim];
     dotp += bval;

     int hash_value = (int) floor(dotp / width);

     return (ullong) hash_value;
}

/**
 * @brief Universal hashing for getting a hash table index from the corresponding hash value tuple
 *
 * @param vector Vector to be hashed
 * @param hash_table Hash table structure
 * @param hash_value Hash value
 * @param index Table index
 */
void lplsh_univhash(Vector *vector, HashTableLP *hash_table, uint *hash_value, uint *index)
{
     uint i;
     ullong hv;
     __uint128_t temp_index = 0;
     __uint128_t temp_hv = 0;

     // computes MinHash values
     for (i = 0; i < hash_table->tuple_size; i++){
          hv = lplsh_compute_hash_value(vector, &hash_table->avec[i * hash_table->dim], hash_table->bval[i], hash_table->width);
          temp_index += ((ullong) hash_table->a[i]) * hv;
          temp_hv += ((ullong) hash_table->b[i]) * hv; 
     }

     // computes 2nd-level hash value and index (universal hash functions)
     *hash_value = (temp_hv % LARGEST_PRIME64);   
     *index = (temp_index % LARGEST_PRIME64) % hash_table->table_size;
}

/**
 * @brief Computes 2nd-level hash value of lists using open 
 *        adressing collision resolution and linear probing.
 * @todo Add other probing strategies.
 *
 * @param vector Vector to be hashed
 * @param hash_table Hash table structure
 *
 * @return index of the hash table
 */ 
uint lplsh_get_index(Vector *vector, HashTableLP *hash_table)
{
     uint checked_buckets, index, hash_value;
     
     lplsh_univhash(vector, hash_table, &hash_value, &index);
     if (hash_table->buckets[index].items.size != 0){ // examine buckets (open adressing)
          if (hash_table->buckets[index].hash_value != hash_value){
               checked_buckets = 1;
               while (checked_buckets < hash_table->table_size){ // linear probing
                    index = ((index + 1) & (hash_table->table_size - 1));
                    if (hash_table->buckets[index].items.size != 0){
                         if (hash_table->buckets[index].hash_value == hash_value)
                              break;   
                    } else {
                         hash_table->buckets[index].hash_value = hash_value; 
                         break;
                    }
                    checked_buckets++;      
               }
               
               if (checked_buckets == hash_table->table_size){
                    fprintf(stderr,"Error: The hash table is full!\n ");
                    exit(EXIT_FAILURE);
               }
          }
     } else {
          hash_table->buckets[index].hash_value = hash_value; 
     }
     
     return index;
}

/**
 * @brief Stores lists in the hash table.
 *
 * @param list List to be hashed
 * @param id ID of the list
 * @param hash_table Hash table
 */ 
uint lplsh_store_vector(Vector *vector, uint id, HashTableLP *hash_table)
{
     uint index;

     // get index of the hash table
     index = lplsh_get_index(vector, hash_table);
     if (hash_table->buckets[index].items.size == 0){ // mark used bucket
          Item new_used_bucket = {index, 1};
          list_push(&hash_table->used_buckets, new_used_bucket);
     }

     // store list id in the hash table
     Item new_item = {id, 1};
     list_push(&hash_table->buckets[index].items, new_item);

     return index;
}

/**
 * @brief Stores lists in the hash table.
 *
 * @param listdb Database of lists to be hashed
 * @param hash_table Hash table
 * @param indices Indices of the used buckets
 */ 
void lplsh_store_vectordb(VectorDB *vectordb, HashTableLP *hash_table, uint *indices)
{
     uint i;
     
     // hash all vectors in the database
     for (i = 0; i < vectordb->size; i++)
          indices[i] = lplsh_store_vector(&vectordb->vectors[i], i, hash_table);
}
