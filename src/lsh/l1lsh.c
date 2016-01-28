/**
 * @file lsh.c
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
#include "mt64.h"
#include "l1lsh.h"

/**
 * @Brief Prints head of a hash table structure
 *
 * @param hash_table Hash table structure
 */
void l1lsh_print_head(HashTable *hash_table)
{
     uint i;

     printf("========== Hash table =========\n");
     printf("Table size: %d\n"
            "Sketch size: %d\n"
            "Max feature value: %d\n"
            "Dimensionality: %d\n"
            "Used buckets: ",
            hash_table->table_size, 
            hash_table->tuple_size,
            hash_table->max_value,
            hash_table->dim); 
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
void l1lsh_print_table(HashTable *hash_table)
{
     uint i;
     
     for (i = 0; i < hash_table->used_buckets.size; i++){
          printf("[  %d  ] ", hash_table->used_buckets.data[i].item);
          list_print(&hash_table->buckets[hash_table->used_buckets.data[i].item].items);
     }
}

/**
 * @brief Initializes a hash table structure for performing L1LSH.
 *
 * @param hash_table Hash table structure
 */
void l1lsh_init(HashTable *hash_table)
{
     hash_table->table_size = 0;
     hash_table->tuple_size = 0; 
     hash_table->dim = 0; 
     hash_table->sample_bits  = NULL;
     hash_table->number_of_samples  = NULL;
     hash_table->buckets = NULL;
     list_init(&hash_table->used_buckets);
     hash_table->a = NULL;
     hash_table->b = NULL;
}

/**
 * @brief Initializes the randon number generator
 */
void l1lsh_rng_init(unsigned long long seed)
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
HashTable l1lsh_create(uint table_size, uint tuple_size, uint dim, uint max_value)
{
     uint i;
     HashTable hash_table;

     hash_table.table_size = table_size;
     hash_table.tuple_size = tuple_size; 
     hash_table.dim = dim;
     hash_table.max_value = max_value; 
     hash_table.sample_bits = (SampleBits *) malloc(tuple_size * sizeof(SampleBits)); 
     hash_table.number_of_samples = (uint *) calloc(dim, sizeof(uint));    
     hash_table.buckets = (Bucket *) calloc(table_size, sizeof(Bucket));
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
 * @param list List to be removed
 * @param hash_table Hash table structure
 */
void l1lsh_erase_from_vector(List *list, HashTable *hash_table)
{  
     uint index = l1lsh_get_index(list, hash_table);
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
void l1lsh_erase_from_index(uint index, HashTable *hash_table)
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
void l1lsh_clear_table(HashTable *hash_table)
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
void l1lsh_destroy(HashTable *hash_table)
{
     free(hash_table->sample_bits);
     free(hash_table->number_of_samples);
     free(hash_table->buckets);
     free(hash_table->a);
     free(hash_table->b);
     list_destroy(&hash_table->used_buckets);
     l1lsh_init(hash_table);
}

/**
 * @brief Picks the sample bits for LSH.
 *
 * @param dim Dimension of the vectors to be hashed
 * @param max_value Largest value in any dimension
 * @param tuple_size Number of MinHash values per tuple
 * @param sample_bits Sample bits
 */

void l1lsh_generate_sample_bits(uint dim, uint max_value,
                              uint tuple_size, SampleBits *sample_bits,
                              uint *number_of_samples)
{
     uint i;
     uint *usedbits = (uint *) calloc(dim * max_value, sizeof(uint));
     
     for(i = 0; i < tuple_size; i++) {
          sample_bits[i].dim = rand() % dim;
          sample_bits[i].loc = rand() % max_value;
          uint bitnum = sample_bits[i].dim * max_value + sample_bits[i].loc;
          while(usedbits[bitnum]){
               sample_bits[i].dim = rand() % dim;
               sample_bits[i].loc = rand() % max_value;
               bitnum = sample_bits[i].dim * max_value + sample_bits[i].loc;
          }
          usedbits[bitnum] = 1;
          number_of_samples[sample_bits[i].dim]++;
	  
     }
     qsort(sample_bits, tuple_size, sizeof(SampleBits), l1lsh_sample_bit_compare);	
} 

/**
 * @brief  Compares two samplebits according to their dimension and location
 *     	   (used as comparison function for the qsort function)
 * @param sb1 Sample bit 1
 * @param sb2 Sample bit 2
 * 
 * @return -1 if sb1 < sb2, 1 if sb1 > sb2 and 0 if sb1 = sb2
 */ 
int l1lsh_sample_bit_compare(void const *a, void const *b)
{
     const SampleBits *sb1 = a;
     const SampleBits *sb2 = b;
     if(sb1->dim < sb2->dim) 
          return -1;
     else if (sb1->dim > sb2->dim) 
          return 1;  
     if(sb1->loc < sb2->loc) 
          return -1;
     else if(sb1->loc > sb2->loc) 
          return 1;
     return 0;
}

/**
 * @brief Computes the hash value of a positive-integer-valued vector according to 
 * 
 * @param vector d-dimensional Euclidian vector
 * @param sbits Location of the sample bits (hyperplanes) 
 *        in each dimension
 * @param numbits Number of sample bits (hyperplanes) for one dimension
 * 
 * @return The hash value of *vector (can be stored in multiple integers)
 */ 
void l1lsh_compute_hash_value(List *list, HashTable *hash_table,
                              uint *hash_value, uint *index)
{
     int low, mid, high, i, k, l, prev_l;
     ullong temp_index = 0;
     ullong temp_hv = 0;
     uint *hv = (uint *) malloc(hash_table->dim * sizeof(uint));

     l = 0;
     for(i = 0; i < hash_table->dim && l < hash_table->tuple_size; i++) { 
          low = l;
          prev_l = l;
          l += hash_table->number_of_samples[i];
          high = l - 1;
          if(hash_table->sample_bits[low].loc > list->data[hash_table->sample_bits[low].dim].freq)
               hv[i] = 0;
          else if(hash_table->sample_bits[high].loc <= list->data[i].freq)
               hv[i] = high - low + 1;
          else{	
               while((low + 1) < high){	
                    mid = (low + high) / 2;
                    if(hash_table->sample_bits[mid].loc <= list->data[i].freq)
                         low = mid;
                    else
                         high = mid;
               }						
               hv[i] = low + 1 - prev_l; 
          }

          temp_index += ((ullong) hash_table->a[i]) * hv[i];
          temp_hv += ((ullong) hash_table->b[i]) * hv[i]; 
     }

     // computes 2nd-level hash value and index (universal hash functions)
     *hash_value = (temp_hv % LARGEST_PRIME);   
     *index = (temp_index % LARGEST_PRIME) % hash_table->table_size;
}

/**
 * @brief Computes 2nd-level hash value of lists using open 
 *        adressing collision resolution and linear probing.
 * @todo Add other probing strategies.
 *
 * @param list List to be hashed
 * @param hash_table Hash table structure
 *
 * @return - index of the hash table
 */ 
uint l1lsh_get_index(List *list, HashTable *hash_table)
{
     uint checked_buckets, index, hash_value;
     
     l1lsh_compute_hash_value(list, hash_table, &hash_value, &index);
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
uint l1lsh_store_list(List *list, uint id, HashTable *hash_table)
{
     uint index;

     // get index of the hash table
     index = l1lsh_get_index(list, hash_table);
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
void l1lsh_store_listdb(ListDB *listdb, HashTable *hash_table, uint *indices)
{
     uint i;
     
         
     // hash all lists in the database
     for (i = 0; i < listdb->size; i++)
          if (listdb ->lists[i].size > 0)
               indices[i] = l1lsh_store_list(&listdb->lists[i], i, hash_table);
}

/**
 * @brief Computes the euclidean distance between two vectors.
 *
 * @param list1 vector 1
 * @param list2 Vector 2
 *
 * 
 * @return Euclidean distance between list1 and list2 (i.e. sqrt((list1 - list2)^2))
 */
double l1lsh_euclidean(List *list1, List *list2)
{
     int i;
     double dist;

     dist = 0;
     for(i = 0; i < list1->size; i++)
          dist += pow(list1->data[i].freq - list2->data[i].freq, 2);

     return sqrt(dist);
}
