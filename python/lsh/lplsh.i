/* lplsh.i */
%module lplsh
%include typemaps.i
%{
#include "lplsh.h"

%}

%pythoncallback;
extern double lplsh_rng_cauchy(void);
extern double lplsh_rng_gaussian(void);
%nopythoncallback;

%ignore lplsh_rng_cauchy;
%ignore lplsh_rng_gaussian;
