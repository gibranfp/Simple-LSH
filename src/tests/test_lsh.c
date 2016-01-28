#include <stdio.h>
#include "listdb.h"
#include "l1lsh.h"
#include "sampledlsh.h"

int main(int argc, char **argv)
{
     ListDB data = listdb_load_from_file(argv[1]);
     
     ListDB coitems = sampledlsh_l1mine(&data, 100, 10, 255, 1024);
     listdb_print(&coitems);
     
     return 0;
}
