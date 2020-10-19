#include "multimalloc.h"
#include <stdio.h>

#define MAX_MALLOC_THREAD_NUM   (32)

static const size_t default_append_size_per_thread = 4096;
static const size_t sys_alignment_size = 4096;

static void *m_heap_ptr;
static uint64_t total_mem_size = 0;
static uint64_t thread_mem_offset[MAX_MALLOC_THREAD_NUM];
static pthread_mutex_t m_malloc_lock;






void *m_malloc_init() {

    if(pthread_mutex_init(&m_malloc_lock, NULL) != 0) {
        fprintf(stderr, "Initialize Mem Lock Failed\n");
        return NULL;
    }

}