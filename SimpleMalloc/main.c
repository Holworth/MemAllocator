#include "malloctest.h"

int main() {
    #ifdef SINGLETHREAD_TEST
        singlethread_test();
    #endif

    #ifdef MULTITHREAD_TEST
        multithread_test();
    #endif
    return 0;
}