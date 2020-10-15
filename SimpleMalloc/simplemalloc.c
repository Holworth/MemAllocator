#include "simplemalloc.h"
#include <stdio.h>
#define ALLOC_ALIGNMENT
#define SYS_ALIGNMENT

// do not put global variables in header file
static const size_t default_append_size  = 4096;
static const size_t alloc_alignment_size = 4;
static const size_t sys_alignment_size   = 4096;
static const size_t default_init_size = 4096;
static void *heap_ptr;
static unsigned int mem_size = 0;
static unsigned int cur_mem_offset = 0;
static pthread_mutex_t mem_lock; 

// ask for allocation for size bytes
// return heap address before sbrk();
void *sys_malloc_alloc(size_t size) {
    void *q = sbrk(0);

    size_t sys_alloc_size = 
    #ifdef SYS_ALIGNMENT
        (size - 1) / sys_alignment_size * sys_alignment_size 
            + sys_alignment_size;
    #else
        size;
    #endif

    void *p = sbrk(sys_alloc_size); 
    if(p == (void*)(-1))
        return NULL;
    else {
        mem_size += sys_alloc_size;
        return q;
    }
}

void *s_malloc_init() {

    if(pthread_mutex_init(&mem_lock, NULL) != 0) {
        fprintf(stderr, "Initialize Mem Lock Failed\n");
        return NULL;
    }
    void *ret = sys_malloc_alloc(default_init_size);
    if(ret){
        heap_ptr = ret;
        mem_size = default_init_size;
        cur_mem_offset = 0;
        return heap_ptr;
    } else {
        return NULL;
    }
}

void *s_malloc(size_t size) {
    size_t alloc_size = 
        #ifdef ALLOC_ALIGNMENT
            (size - 1) / alloc_alignment_size * alloc_alignment_size 
                       + alloc_alignment_size;
        #else
            size;
        #endif
    pthread_mutex_lock(&mem_lock); 
    if(cur_mem_offset + alloc_size > mem_size) {
        // ask for memory from system
        // may not be aligned
        size_t sys_alloc_size = max(default_append_size, alloc_size);
        void *new_heap = sys_malloc_alloc(sys_alloc_size);
        if(new_heap == NULL) {
            fprintf(stderr, "malloc failed\n");
            pthread_mutex_unlock(&mem_lock);
            return NULL;
        } 
    }

    char *ret = (char *)heap_ptr + cur_mem_offset;
    cur_mem_offset += alloc_size;
    pthread_mutex_unlock(&mem_lock);
    return (void *)ret;
}
