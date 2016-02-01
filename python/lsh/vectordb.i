/* vectordb.i */
%module vectordb
%include typemaps.i
%include "exception.i"

%{
#include "array_lists.h"
#include "vectors.h"
#include "vectordb.h"
#include <assert.h>

     static int errVec = 0;

     %}
 
extern void vectordb_init(VectorDB *);
extern VectorDB vectordb_create(int, int);
extern void vectordb_clear(VectorDB *);
extern void vectordb_destroy(VectorDB *);
extern void vectordb_push(VectorDB *vectordb, Vector *vector);
extern void vectordb_print(VectorDB *);
extern void vectordb_print_multi(VectorDB *, List *);
extern void vectordb_print_range(VectorDB *, uint, uint);
extern VectorDB vectordb_load_from_file(char *);
extern void vectordb_save_to_file(char *, VectorDB *);
extern void vectordb_apply_to_all(VectorDB *, void (*)(Vector *));

typedef struct VectorDB{
     uint size;
     uint dim;
     Vector *vectors;
} VectorDB;

typedef unsigned int uint;

%exception VectorDB::__getitem__ {
     assert(!errVec);
     $action
          if (errVec) {
               errVec = 0; // clear flag for next time
               SWIG_exception(SWIG_IndexError, "Index out of bounds");
          }
}

%extend VectorDB {
     Vector __getitem__(uint pos) {
          if (pos >= $self->size) {
               errVec = 1;
               Vector vector = (const Vector) {0};
               return vector;
          }
          
          return $self->vectors[pos];
     }

     void push(uint pos, uint dim, double value) {
          if (pos >= $self->size) {
               errVec = 1;
          } else{
               Dim newdim = {dim, value};
               vector_push($self->vectors + pos, newdim);
          }
     }

     void push(uint pos, Dim dim) {
          if (pos >= $self->size) {
               errVec = 1;
          } else{
               vector_push($self->vectors + pos, dim);
          }
     }
}
