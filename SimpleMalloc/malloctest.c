#include "malloctest.h"


const size_t max_thread_num = 10;
static inline uint64_t time_of_micros(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

void singlethread_malloc() {

    pthread_t tid;
    tid = pthread_self();
    // set rand seed
    srand((unsigned int)tid);
    void *ret = NULL;
    size_t idx = 1;

    uint64_t start_time = time_of_micros();

    for(; idx <= 10000; ++idx) {
        size_t malloc_size = rand() % 1024;
        ret = s_malloc(malloc_size);
        if(ret == NULL) {
            fprintf(stderr, "failed to malloc");
            break;
        }
    }

    uint64_t end_time = time_of_micros();
    printf("tid:%u\n  time passed:%.8lfs\n  each %.4lfus\n", (unsigned int)tid,
                (double)(end_time-start_time)/1000000,
                (double)(end_time-start_time)/idx);
    
}

void singlethread_test() {
    void *p = s_malloc_init();
    if(p == NULL) return;
    singlethread_malloc();
}

void multithread_test() {
    void *p = s_malloc_init();
    if(p == NULL) return;

    pthread_t tids[max_thread_num];
    for(size_t idx = 0; idx < max_thread_num; ++idx){
        int ret = pthread_create(&tids[idx], NULL, singlethread_malloc, NULL);
        if(ret != 0) {
            fprintf(stderr, "pthread create error\n");
        }
    }
}