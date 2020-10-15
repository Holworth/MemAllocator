#include "simplemalloc.h"
#include <stdio.h>

// ask for allocation for size bytes
// return heap address before sbrk();
void *sys_malloc_alloc(size_t size) {
    void *q = sbrk(0);
    void *p = sbrk(size); 
    if(p == (void*)(-1))
        return NULL;
    else 
        return q;
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
    pthread_mutex_lock(&mem_lock); 
    if(cur_mem_offset + size > mem_size) {
        size_t alloc_size = max(default_append_size, size);
        #ifdef ALIGNMENT
        alloc_size = (alloc_size - 1) / alignment_size * alignment_size 
                                        + alignment_size;
        #endif
        void *new_heap = sys_malloc_alloc(alloc_size);
        if(new_heap == NULL) {
            fprintf(stderr, "malloc failed\n");
            return NULL;
        } else {
            mem_size += alloc_size;
        }
    }

    char *ret = (char *)heap_ptr + cur_mem_offset;
    cur_mem_offset += size;
    pthread_mutex_unlock(&mem_lock);
    return (void *)ret;
}
