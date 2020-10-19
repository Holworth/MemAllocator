#include "malloctest.h"
#include <string.h>

int main(int argc, char **argv) {
    s_malloc_init();
    run_test(argc, argv);
    s_malloc_print();
    return 0;
}