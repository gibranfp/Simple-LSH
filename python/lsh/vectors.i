/* array_lists.i */
%module vectors
%include typemaps.i
%include "exception.i"
%{
     #include "array_lists.h"
     #include "vectors.h"
     #include <assert.h>
     static int errDim = 0;
%}


typedef struct Dim {
     uint dim;
     double value;
} Dim;


typedef struct Vector {
     uint size;
     Dim *data;
} Vector;

typedef unsigned int uint;

%pythoncallback;
extern void vector_init(Vector *);
extern Vector vector_create(uint);
extern void vector_destroy(Vector *);
extern void vector_print(Vector *);
extern void vector_print_multi(Vector *, List *);
extern void vector_print_range(Vector *, uint, uint);
extern void vector_push(Vector *, Dim);
extern void vector_pop(Vector *);
extern Vector vector_duplicate(Vector *);
%nopythoncallback;

%ignore vector_init;
%ignore vector_create;
%ignore vector_destroy;
%ignore vector_print;
%ignore vector_print_multi;
%ignore vector_print_range;
%ignore vector_push;
%ignore vector_pop;
%ignore vector_duplicate;

%exception Vector::__getitem__ {
    assert(!errDim);
    $action
    if (errDim) {
        errDim = 0; // clear flag for next time
        SWIG_exception(SWIG_IndexError, "Index out of bounds");
        }
    }

%extend Vector {
    Dim  __getitem__(size_t i) {
        if (i >= $self->size) {
            errDim = 1;
            Dim dim = (const Dim) {0};
            return dim;
        }
        return $self->data[i];
    }
}
