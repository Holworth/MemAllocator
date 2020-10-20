#include "simplemalloc.h"
#include "atomic_lock.h"
#include <stdio.h>
#define S_ALLOC_ALIGNMENT
#define S_SYS_ALIGNMENT

// do not put global variables in header file !!!
static const size_t default_append_size  = 4096;
static const size_t alloc_alignment_size = 4;
static const size_t sys_alignment_size   = 4096;
static const size_t default_init_size = 4096;
static void *heap_ptr;
// heap point
static unsigned int mem_size = 0;
// how much memory has been allocate: include alignment
static unsigned int cur_mem_offset = 0;
// how much memory acctually used
// there are fragment between actually usable memory and allocated memory
static unsigned int cur_mem_usable = 0;

#if ATOMIC_SPIN
volatile atomic_lock_t lock;
#else
static pthread_mutex_t mem_lock; 
#endif

// ask for allocation for size bytes
// return heap address before sbrk();
void *sys_malloc_alloc(size_t size) {
    void *q = sbrk(0);

    size_t sys_alloc_size = 
    #ifdef S_SYS_ALIGNMENT
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

    #if ATOMIC_SPIN
        atomic_spin_init(&lock);
    #else
        if(pthread_mutex_init(&mem_lock, NULL) != 0) {
            fprintf(stderr, "Initialize Mem Lock Failed\n");
            return NULL;
        }
    #endif

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
        #ifdef S_ALLOC_ALIGNMENT
            (size - 1) / alloc_alignment_size * alloc_alignment_size 
                        + alloc_alignment_size;
        #else
            size;
        #endif
    #if ATOMIC_SPIN
        atomic_spin_lock(&lock);
    #else
        pthread_mutex_lock(&mem_lock); 
    #endif
    if(cur_mem_offset + alloc_size > mem_size) {
        // ask for memory from system
        // may not be aligned
        size_t sys_alloc_size = max(default_append_size, alloc_size);
        void *new_heap = sys_malloc_alloc(sys_alloc_size);
        if(new_heap == NULL) {
            fprintf(stderr, "malloc failed\n");
            #if ATOMIC_SPIN
                atomic_spin_unlock(&lock);
            #else
                pthread_mutex_unlock(&mem_lock);
            #endif
            return NULL;
        } 
    }

    char *ret = (char *)heap_ptr + cur_mem_offset;
    cur_mem_offset += alloc_size;
    // add size means these memory are actually usable
    cur_mem_usable += size;
    #if ATOMIC_SPIN
        atomic_spin_unlock(&lock);
    #else
        pthread_mutex_unlock(&mem_lock);
    #endif
    return (void *)ret;
}

void s_malloc_print(){

    printf("[Heap Base]: %p\n", heap_ptr);
    printf("[Heap Top ]: %p\n", heap_ptr+mem_size);
    printf("[System Memory Allocated]: %u Bytes(%.6lf KB)\n", mem_size, 
                                                            (double)mem_size / 1024);
    printf("[Memory Actually Allocated]: %u Bytes(%.6lf KB)\n", cur_mem_offset,
                                                            (double)cur_mem_offset / 1024);
    printf("[User Memory require]: %u Bytes(%.6lf KB)\n", cur_mem_usable, 
                                                            (double)cur_mem_usable / 1024);
    printf("Usable Percentage :%.4lf%%\n", (double)cur_mem_usable/cur_mem_offset * 100);
}

