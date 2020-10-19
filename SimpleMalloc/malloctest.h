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


typedef struct thread_param {
    uint64_t max_malloc_times_;
    int64_t malloc_size_seed_;
} thread_param_t;

void multithread_test(int thread_num);
void *thread_task(void *param);
void run_test(int argc, char **argv);

#endif