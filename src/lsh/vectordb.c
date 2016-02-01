/**
 * @file vectordb.c
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
 * @brief Basic operations on databases of vectors.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <float.h>
#include "vectordb.h"

/**
 * @brief Initializes a vector database structure to zero
 *
 * @param *vectordb Vector database to initialize
 */
void vectordb_init(VectorDB *vectordb)
{     
     vectordb->size = 0;
     vectordb->dim = 0;
     vectordb->vectors = NULL;
}

/**
 * @brief Creates a vector database structure of a given size with vectors initialized to zero
 *
 * @param size Size of the database to create
 *
 * @return Created database
 */
VectorDB vectordb_create(uint size, uint dim)
{     
     VectorDB vectordb;
     vectordb.size = size;
     vectordb.dim = dim;
     vectordb.vectors = (Vector *) calloc(size, sizeof(Vector));

     return vectordb;
}

/**
 * @brief Clears a vector database structure
 *
 * @param *vectordb Vector database to be cleared
 */
void vectordb_clear(VectorDB *vectordb)
{     
     free(vectordb->vectors);
     vectordb_init(vectordb);
}

/**
 * @brief Destroys a vector database structure
 *
 * @param *vectordb Vector database to be destroyed
 */
void vectordb_destroy(VectorDB *vectordb)
{     
     int i;

     for (i = 0; i < vectordb->size; i++)
          vector_destroy(&vectordb->vectors[i]);

     free(vectordb->vectors);
     vectordb_init(vectordb);
}

/**
 * @brief Prints a database of vectors
 *
 * @param vectordb Database to be printed
 */
void vectordb_print(VectorDB *vectordb)
{     
     uint i;
     for (i = 0; i < vectordb->size; i++) {
          printf("[  %d  ] ", i);
          vector_print(&vectordb->vectors[i]);
     }
}

/**
 * @brief Prints given positions of a database of vectors
 *
 * @param vectordb Database to be printed
 * @param positions Positions of the database to be printed
 */
void vectordb_print_multi(VectorDB *vectordb, List *positions)
{
     uint i, pos;
     
     for (i = 0; i < positions->size; i++) {
          pos = positions->data[i].item;
          if (pos >= 0 && pos < vectordb->size){
               printf("[  %d  ] ", pos);
               vector_print(&vectordb->vectors[pos]);
          }
          else
               printf ("%d-OOR ", pos);
     }
     printf ("\n");
}

/**
 * @brief Prints a given range of a database of vectors
 *
 * @param *vectordb Database to be printed
 * @param low Start position of the range
 * @param high End position of the range
 */
void vectordb_print_range(VectorDB *vectordb, uint low, uint high)
{     
     uint i;
     if (low >= 0 && low <= high && high < vectordb->size) {
          printf ("%d -- %d-%d\n", vectordb->size, low, high);
          for (i = low; i <= high; i++){
               printf("[  %d  ] ", i);
               vector_print(&vectordb->vectors[i]); 
          }
     } else {
          printf ("Invalid range %d-%d. Database range is 0-%d", 
                  low, high, vectordb->size - 1);
     }
}

/**
 * @brief Applies a function to all the vectors in a database
 *
 * @param *vectordb Database where the function will be applied
 * @param *func Function to apply
 */
void vectordb_apply_to_all(VectorDB *vectordb, void (*func)(Vector *))
{
     uint i;
     for (i = 0; i < vectordb->size; i++)
          func(&vectordb->vectors[i]);
}

/**
 * @brief Applies a function to a range of vectors in a database
 *
 * @param *vectordb Database where the function will be applied
 * @param *func Function to apply
 * @param low Start of the range
 * @param high End of the range
 */
void vectordb_apply_to_range(VectorDB *vectordb, void (*func)(Vector *), uint low,
                           uint high)
{
     uint i;
     if (low >= 0 && low <= high && high < vectordb->size) {
          for (i = low; i <= high; i++)
               func(&vectordb->vectors[i]);
     } else {
          printf ("%d-%d-OOR\n", low, high);
     }
}

/**
 * @brief Applies a function to certain of vectors in a database
 *
 * @param *vectordb Database where the function will be applied
 * @param *func Function to apply
 * @param *positions Positions of the vectors where the function will be applied
 */
void vectordb_apply_to_multi(VectorDB *vectordb, void (*func)(Vector *), List *positions)
{
     uint i, pos;
     for (i = 0; i < positions->size; i++) {
          pos = positions->data[i].item;
          if (pos >= 0 && pos < vectordb->size)
               func(&vectordb->vectors[pos]);
          else
               printf ("Position %d OOR\n", pos);
     }
}

/**
 * @brief Adds a new vector to the end of a database
 *
 * @param *vectordb Database where the vector will be added
 * @param vector vector to be added
 */
void vectordb_push(VectorDB *vectordb, Vector *vector)
{
     uint newsize = vectordb->size + 1;
     vectordb->vectors = realloc(vectordb->vectors, newsize * sizeof(Vector));
     vectordb->vectors[vectordb->size] = *vector;
     vectordb->size = newsize;
}

/**
 * @brief Deletes the vector at the end of a database
 *
 * @param *vectordb Database where the vector will be deleted
 */
void vectordb_pop(VectorDB *vectordb)
{
     vectordb->size--;
     vector_destroy(&vectordb->vectors[vectordb->size]);
     vectordb->vectors = realloc(vectordb->vectors, vectordb->size * sizeof(Vector));
}

/**
 * @brief Deletes multiple vectors at the end of a database
 *
 * @param *vectordb Vector database where the vectors will be deleted
 * @param number Number of vectors to be removed
 */
void vectordb_pop_multi(VectorDB *vectordb, uint number)
{
     vectordb_apply_to_range(vectordb, vector_destroy, vectordb->size - number - 1, vectordb->size - 1);
     vectordb->size -= number;
     vectordb->vectors = realloc(vectordb->vectors, vectordb->size * sizeof(Vector));
}

/**
 * @brief Deletes multiple vectors at the end of a database
 *
 * @param *vectordb Vector database where the vectors will be deleted
 * @param last Last vector to be removed
 */
void vectordb_pop_until(VectorDB *vectordb, uint last)
{
     vectordb_apply_to_range(vectordb, vector_destroy, last, vectordb->size - 1);
     vectordb->size = last;
     vectordb->vectors = realloc(vectordb->vectors, vectordb->size * sizeof(Vector));
}

/**
 * @brief Deletes a vector in a given position in a database
 *
 * @param *vectordb Database where the vector will be deleted
 * @param position Position of the vector to be deleted
 */
void vectordb_delete_position(VectorDB *vectordb, uint position)
{
     vector_destroy(&vectordb->vectors[position]);
     uint newsize = vectordb->size - 1;
     Vector *tmpvectors = (Vector *) malloc(newsize * sizeof(Vector));
     memcpy(tmpvectors, vectordb->vectors, position * sizeof(Vector));
     memcpy(tmpvectors + position, vectordb->vectors + position + 1, 
            (newsize - position) * sizeof(Vector));
     free(vectordb->vectors);
     vectordb->vectors = tmpvectors;
     vectordb->size = newsize;
}

/**
 * @brief Deletes vectors in a given range of a database
 *
 * @param *vectordb Database where the vectors will be deleted
 * @param low Start position of the range
 * @param high End position of the range
 */
void vectordb_delete_range(VectorDB *vectordb, uint low, uint high)
{    
     vectordb_apply_to_range(vectordb, vector_destroy, low, high);
     uint range = high - low + 1;
     uint newsize = vectordb->size - range;
     Vector *tmpvectors = (Vector *) malloc(newsize * sizeof(Vector));
     memcpy(tmpvectors, vectordb->vectors, low * sizeof(Vector));
     memcpy(tmpvectors + low, vectordb->vectors + high + 1, 
            (vectordb->size - high - 1) * sizeof(Vector));
     free(vectordb->vectors);
     vectordb->vectors = tmpvectors;
     vectordb->size = newsize;
}

/**
 * @brief Inserts a vector in a given position in a database
 *
 * @param *vectordb Database where the vector will be inserted
 * @param *vector Vector to be inserted
 * @param position Position where the vector is to be inserted
 */
void vectordb_insert(VectorDB *vectordb, Vector *new_vector, uint position)
{
     uint newsize = vectordb->size + 1;
     Vector *tmpvectors;
     tmpvectors = (Vector *) malloc(newsize * sizeof(Vector));
     memcpy(tmpvectors, vectordb->vectors, position * sizeof(Vector));
     tmpvectors[position] = *new_vector;
     if (position < vectordb->size)
          memcpy(tmpvectors + position + 1, vectordb->vectors + position, 
                 (vectordb->size - position) * sizeof(Vector));

     free(vectordb->vectors);
     vectordb->vectors = tmpvectors;
     vectordb->size = newsize;
}

/**
 * @brief Appends one vector database to another
 *
 * @param vectordb1 Base vector database
 * @param vectordb2 Vector database to be appended
 */
void vectordb_append(VectorDB *vectordb1, VectorDB *vectordb2)
{
     uint newsize = vectordb1->size + vectordb2->size;

     vectordb1->vectors = realloc(vectordb1->vectors, newsize * sizeof(Vector));
     memcpy(vectordb1->vectors + vectordb1->size, vectordb2->vectors, vectordb2->size * sizeof(Vector));
     vectordb1->size = newsize;
}

/**
 * @brief Loads a vector database from a file
 *        Format: 
 *             size item_1:value_1 item_2:value_2 ... item_size:value_size
 *                        ...
 *
 * @param filename File containing the inverted file index of vectors
 *
 * @return Vector database
 */
VectorDB vectordb_load_from_file(char *filename)
{
     FILE *file;
     if (!(file = fopen(filename,"r"))) {
          fprintf(stderr,"Error: Could not open file %s\n", filename);
          exit(EXIT_FAILURE);
     }

     // checking the number of vectors in the file
     size_t len = 0;
     ssize_t read;
     char *line = NULL;
     VectorDB vectordb;
     vectordb.size = 0;
     while ((read = getline(&line, &len, file)) != -1) {
          if ('\n' != line[0])
               vectordb.size++;
     }
     rewind(file);

     // reading vectors
     vectordb.dim = 0;
     vectordb.vectors = (Vector *) malloc(vectordb.size * sizeof(Vector));

     uint i, j;
     for (i = 0; i < vectordb.size; i ++) {
          fscanf(file,"%u", &vectordb.vectors[i].size);
          vectordb.vectors[i].data = (Dim *) malloc(vectordb.vectors[i].size * sizeof(Dim));
          for (j = 0; j < vectordb.vectors[i].size; j++) {
               char sep;
               fscanf(file,"%u%c%lf", &vectordb.vectors[i].data[j].dim, &sep, &vectordb.vectors[i].data[j].value);
               if (vectordb.dim < vectordb.vectors[i].data[j].dim + 1)
                    vectordb.dim = vectordb.vectors[i].data[j].dim + 1;
          }
     }
     
     if (fclose(file)) {
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
     
     return vectordb;
}

/**
 * @brief Saves a vector database in a file.
 *        Format: 
 *             size item_1:value_1 item_2:value_2 ... item_size:value_size
 *
 * @param filename File where the inverted file index will be saved
 * @param vectordb Vector database to save
 */
void vectordb_save_to_file(char *filename, VectorDB *vectordb)
{
     FILE *file;     
     if (!(file = fopen(filename,"w"))) {
          fprintf(stderr,"Error: Could not create file %s\n", filename);
          exit(EXIT_FAILURE);
     }

     uint i, j;
     for (i = 0; i < vectordb->size; i++) {
          fprintf(file,"%u", vectordb->vectors[i].size);
          for (j = 0; j < vectordb->vectors[i].size; j++)
               fprintf(file," %u:%lf", vectordb->vectors[i].data[j].dim, vectordb->vectors[i].data[j].value);
          fprintf(file,"\n");
     }

     if (fclose(file)) {
          fprintf(stderr,"Error: Could not close file %s\n", filename);
          exit(EXIT_FAILURE);
     }
}
