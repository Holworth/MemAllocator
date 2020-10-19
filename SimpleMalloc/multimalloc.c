#include "multimalloc.h"
#include <stdio.h>

#define MAX_MALLOC_THREAD_NUM   (32)

static const size_t default_append_size_per_thread = 4096;
static const size_t m_sys_alignment_size = 4096;
static const size_t m_alloc_alignment_size = 4;
static const size_t init_sys_alloc_size = default_append_size_per_thread * MAX_MALLOC_THREAD_NUM;

static void *m_heap_ptr;
static uint64_t total_mem_size = 0;
static uint64_t thread_mem_offset[MAX_MALLOC_THREAD_NUM];
static uint64_t thread_mem_base[MAX_MALLOC_THREAD_NUM];
static uint64_t thread_mem_len[MAX_MALLOC_THREAD_NUM];
static uint64_t thread_mem_next_base[MAX_MALLOC_THREAD_NUM];
static uint64_t thread_mem_next_len[MAX_MALLOC_THREAD_NUM];
static pthread_mutex_t m_malloc_lock;


void *sys_malloc_alloc_m(size_t size) {

    void *cur_heap_top = sbrk(0);

    size_t sys_alloc_size = 
    #ifdef SYS_ALIGNMENT
        (size - 1) / m_sys_alignment_size * m_sys_alignment_size
        + m_sys_alignment_size;
    #else
        size;
    #endif

    void *p = sbrk(sys_alloc_size);
    if(p == (void *)(-1))
        return NULL;
    else return cur_heap_top;
}




void *m_malloc_init() {

    if(pthread_mutex_init(&m_malloc_lock, NULL) != 0) {
        fprintf(stderr, "Initialize Mem Lock Failed\n");
        return NULL;
    }

    void *ret = sys_malloc_alloc_m(init_sys_alloc_size);
    if(ret) {
        m_heap_ptr = ret;
        total_mem_size = init_sys_alloc_size;
    } else {
        return NULL;
    }

    for(int i = 0; i < MAX_MALLOC_THREAD_NUM; ++i) {
        thread_mem_base[i] = (uint64_t)m_heap_ptr + i * default_append_size_per_thread;
        thread_mem_offset[i] = 0;
        thread_mem_next_base[i] = 0;
        thread_mem_len[i] = default_append_size_per_thread;
    }
    return m_heap_ptr;
}

void *m_malloc(int tid, size_t size) {
    size_t alloc_size = 
        #ifdef ALLOC_ALIGNMENT
            (size - 1) / m_alloc_alignment_size * m_alloc_alignment_size
            + m_alloc_alignment_size;
        #else
            size;
        #endif
    
    if(thread_mem_offset[tid] + alloc_size > thread_mem_len) {
        // system malloc new memory;
        // if new memory is already allocated:
        pthread_mutex_lock(&m_malloc_lock);
        if(thread_mem_next_base[tid] != 0 && thread_mem_next_len[tid] >= alloc_size) {
            thread_mem_base[tid] = thread_mem_next_base[tid];
            thread_mem_next_base[tid] = 0;
            thread_mem_offset[tid] = 0;
        } else { 
        // no new memory or new memory is not enough;
            size_t sys_alloc_size_per_thread = max(default_append_size_per_thread, alloc_size);
            void *new_heap = sys_malloc_alloc_m(sys_alloc_size_per_thread * MAX_MALLOC_THREAD_NUM);
            if(new_heap == NULL) {
                fprintf(stderr, "system malloc (sbrk) failed\n");
                pthread_mutex_unlock(&m_malloc_lock);
                return NULL;
            }
            for(int i = 0; i < MAX_MALLOC_THREAD_NUM; ++i) {
                thread_mem_next_base[i] = new_heap + i * sys_alloc_size_per_thread;
                thread_mem_next_len[i] = sys_alloc_size_per_thread;
            }
            thread_mem_base[tid] = thread_mem_next_base[tid];
            thread_mem_next_base[tid] = 0;
            thread_mem_offset[tid] = 0;
        }
        pthread_mutex_unlock(&m_malloc_lock);
    }

    char *ret = thread_mem_base[tid] + thread_mem_offset[tid];
    thread_mem_offset[tid] += size;
    return (void *)ret;
}