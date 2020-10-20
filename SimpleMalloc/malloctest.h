#ifndef MALLOCTEST_H_
#define MALLOCTEST_H_

#include "simplemalloc.h"
#include "multimalloc.h"
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
//#define SINGLETHREAD_TEST
#define MAX_TEST_THREAD_NUM     (32)

typedef enum malloc_type { 
    simple_malloc, 
    multi_malloc 
} malloc_type_t;

typedef enum malloc_test_type {
    exec_time_test,
}malloc_test_type_t;

typedef struct thread_param {
    uint64_t max_malloc_times_;
    int64_t malloc_size_seed_;
    malloc_type_t thread_malloc_type;
    int tid;
} thread_param_t;

void multithread_test(int thread_num);
void *thread_task_exectime_test(void *param);
void run_test(int argc, char **argv);


#endif