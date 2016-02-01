#include <stdio.h>
#include "mt64.h"
#include "lplsh.h"
#include "sampledlsh.h"

void test_l1lsh(char *input)
{
     ListDB data = listdb_load_from_file(input);
     
     ListDB coitems = sampledlsh_l1mine(&data, 50, 10, 255, 1024);
     listdb_print(&coitems);
}

void test_lplsh_cauchy(char *input)
{
     VectorDB data = vectordb_load_from_file(input);
     
     ListDB coitems = sampledlsh_lpmine(&data, 10, 10, 4.0, 1024, lplsh_rng_cauchy);
     listdb_print(&coitems);
}

void test_lplsh_gaussian(char *input)
{
     VectorDB data = vectordb_load_from_file(input);
     
     ListDB coitems = sampledlsh_lpmine(&data, 25, 10, 2.0, 1024, lplsh_rng_gaussian);
     listdb_print(&coitems);
}

int main(int argc, char **argv)
{
          //initialize Mersenne Twister random number generator
     unsigned long long init[4]={0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL}, length=4;
     init_by_array64(init, length);

     /* test_l1lsh(argv[1]); */
     test_lplsh_gaussian(argv[1]);
     /* test_lplsh_cauchy(argv[1]); */
     
     return 0;
}
