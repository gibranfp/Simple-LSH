/* sampledlsh.i */
%module sampledlsh
%{
#include "listdb.h"
#include "sampledlsh.h"

#define DEF_TABLE_SIZE 524288
%}

extern ListDB sampledlsh_l1mine(ListDB *, uint, uint, uint, uint);
