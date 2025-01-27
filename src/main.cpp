#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug/debug.h"
#include "parser/chunk.h"
#include "vm/vm.h"
#include "helper/flags.h"

int main(int argc, char *argv[])
{
    flags(argc, argv);
    init_vm();

    run_file(argv[1]);

    free_vm();
    return 0;
}