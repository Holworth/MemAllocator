#ifndef MULTIMALLOC_H_
#define MULTIMALLOC_H_
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>
#define max(a, b) ((a) > (b)) ? (a) : (b)
#define min(a, b) ((a) < (b)) ? (a) : (b)

/*  m_malloc means multi(thread) malloc
    which only supports append-only malloc,
    but faster in multi-thread condition, compared
    with simple malloc
*/

void *m_malloc_init();
void *m_malloc(size_t size);
void *sys_malloc_alloc_m(size_t size);
void m_malloc_print();

#endif