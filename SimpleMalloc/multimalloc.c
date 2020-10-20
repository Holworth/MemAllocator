#include "multimalloc.h"
#include <stdio.h>

#define M_SYS_ALIGNMENT
#define M_ALLOC_ALIGNMENT

static const size_t default_append_size_per_thread = 8192;
static const size_t m_sys_alignment_size = 4096;
static const size_t m_alloc_alignment_size = 4;
static const size_t init_sys_alloc_size = default_append_size_per_thread * MAX_MALLOC_THREAD_NUM;
static const size_t bytes_per_MB = 1 << 20;

static void *m_heap_ptr;
static uint64_t total_mem_size = 0;
static uint64_t total_mem_usable = 0;
// offset of current allocated interval
static uint64_t thread_mem_offset[MAX_MALLOC_THREAD_NUM];
// base address of current allocated interval
static uint64_t thread_mem_base[MAX_MALLOC_THREAD_NUM];
// length of current allocated interval
static uint64_t thread_mem_len[MAX_MALLOC_THREAD_NUM];
// base address of the next (possible) allocated interval
static uint64_t thread_mem_next_base[MAX_MALLOC_THREAD_NUM];
// length of next (possible) allocated interval
static uint64_t thread_mem_next_len[MAX_MALLOC_THREAD_NUM];
// following two arrays are used for performance test
// how many bytes each thread use: User using m_malloc to allocate
static uint64_t thread_usable_mem[MAX_MALLOC_THREAD_NUM];
// how many bytes each thread ask from the system
static uint64_t thread_sys_alloc_mem[MAX_MALLOC_THREAD_NUM];
static pthread_mutex_t m_malloc_lock;
static uint64_t m_sys_alloc_times = 0;
int m_malloc_thread_num = 1;


// sys_malloc_alloc_m is only allowed used
// after the thread gets the lock(unless in init condition)
void *sys_malloc_alloc_m(size_t size) {

    void *cur_heap_top = sbrk(0);

    size_t sys_alloc_size = 
    #ifdef M_SYS_ALIGNMENT
        (size - 1) / m_sys_alignment_size * m_sys_alignment_size
        + m_sys_alignment_size;
    #else
        size;
    #endif

    void *p = sbrk(sys_alloc_size);
    if(p == (void *)(-1))
        return NULL;
    else {
        total_mem_size += sys_alloc_size;
        return cur_heap_top;
    }
}




void *m_malloc_init() {

    if(pthread_mutex_init(&m_malloc_lock, NULL) != 0) {
        fprintf(stderr, "Initialize Mem Lock Failed\n");
        return NULL;
    }

    void *ret = sys_malloc_alloc_m(init_sys_alloc_size);
    if(ret) {
        m_heap_ptr = ret;
    } else {
        return NULL;
    }

    for(int i = 0; i < m_malloc_thread_num; ++i) {
        thread_mem_base[i] = (uint64_t)m_heap_ptr + i * default_append_size_per_thread;
        thread_mem_offset[i] = 0;
        thread_mem_len[i] = default_append_size_per_thread;
        thread_mem_next_base[i] = 0;
        thread_mem_next_len [i] = 0;
        thread_usable_mem[i] = 0;
        thread_sys_alloc_mem[i] = default_append_size_per_thread;
    }
    m_sys_alloc_times = 1;
    total_mem_usable = 0;
    return m_heap_ptr;
}

static inline void switch_to_next_interval(int tid) {
    thread_mem_base[tid] = thread_mem_next_base[tid];
    thread_mem_len [tid] = thread_mem_next_len[tid];
    thread_mem_offset[tid] = 0;
    thread_mem_next_base[tid] = 0;
    thread_mem_next_len [tid] = 0;
}

void *m_malloc(int tid, size_t size) {
    size_t alloc_size = 
        #ifdef M_ALLOC_ALIGNMENT
            (size - 1) / m_alloc_alignment_size * m_alloc_alignment_size
            + m_alloc_alignment_size;
        #else
            size;
        #endif
    
    if(thread_mem_offset[tid] + alloc_size > thread_mem_len[tid]) {
        // system malloc new memory;
        // if new memory is already allocated:
        pthread_mutex_lock(&m_malloc_lock);
        if(thread_mem_next_base[tid] != 0 && thread_mem_next_len[tid] >= alloc_size) {
            // directly switch to the next interval
            switch_to_next_interval(tid);
        } else { 
            // no new memory or new memory is not enough;
            // allocate new memory first
            size_t sys_alloc_size_per_thread = max(default_append_size_per_thread, alloc_size);
            void *new_heap = sys_malloc_alloc_m(sys_alloc_size_per_thread * m_malloc_thread_num);
            m_sys_alloc_times += 1;

            if(new_heap == NULL) {
                fprintf(stderr, "system malloc (sbrk) failed\n");
                pthread_mutex_unlock(&m_malloc_lock);
                return NULL;
            }
            // set the next array;
            for(int i = 0; i < m_malloc_thread_num; ++i) {
                thread_mem_next_base[i] = (uint64_t)new_heap + i * sys_alloc_size_per_thread;
                thread_mem_next_len[i] = sys_alloc_size_per_thread;
                // system malloc monitor
                thread_sys_alloc_mem[i] += sys_alloc_size_per_thread;
            }

            switch_to_next_interval(tid);
        }
        pthread_mutex_unlock(&m_malloc_lock);
    }
    // allocate new memory
    char *ret = (char *)thread_mem_base[tid] + thread_mem_offset[tid];
    thread_mem_offset[tid] += alloc_size;
    thread_usable_mem[tid] += size;
    return (void *)ret;
}

// Alert: m_malloc_print must be used after m_malloc_init executes;
void m_malloc_print() {
    pthread_mutex_lock(&m_malloc_lock);

    for(int i = 0; i < m_malloc_thread_num; ++i) {
        total_mem_usable += thread_usable_mem[i];
    }
    printf("[Total Mem Usable Percentage]: %.4lf%%\n", (double)total_mem_usable / total_mem_size * 100);
    printf(">>>[Total Mem System Allocated]: %.6lfMB(%luBytes)\n", (double)total_mem_size / bytes_per_MB, total_mem_size);
    printf(">>>[Total Mem User Ask]: %.6lfMB(%luBytes)\n", (double)total_mem_usable / bytes_per_MB, total_mem_usable);
    printf(">>>[System Malloc Times]:%lu\n", m_sys_alloc_times);
    pthread_mutex_unlock(&m_malloc_lock);

    for(int i = 0; i < m_malloc_thread_num; ++i) {
        assert(thread_sys_alloc_mem[i] != 0);
        printf("\n\n");
        printf("[tid %2d Mem Usable Percentage]: %.4lf%%\n", i, 
                            (double)thread_usable_mem[i] / thread_sys_alloc_mem[i] * 100);
        printf(">>>[tid %2d Mem System Allocated]: %.6lfMB(%luBytes)\n", i, (double)thread_sys_alloc_mem[i] / bytes_per_MB, \
                                                                                thread_sys_alloc_mem[i]);
        printf(">>>[tid %2d Mem User Ask]: %.6lfMB(%luBytes)\n", i, (double)thread_usable_mem[i] / bytes_per_MB, \
                                                                    thread_usable_mem[i]);
    }
    return;
}