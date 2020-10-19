#ifndef SIMPLEMALLOC_H_
#define SIMPLEMALLOC_H_
#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>
#define max(a, b) ((a) > (b)) ? (a) : (b)

/*
    s_malloc means simple malloc,
    which only supports malloc, to be more specific,
    append-only malloc.
*/
void *s_malloc_init();
void *s_malloc(size_t size);
void *sys_malloc_alloc(size_t size);
void s_malloc_print();

#endif
