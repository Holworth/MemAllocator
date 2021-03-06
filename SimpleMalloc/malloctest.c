#include "malloctest.h"
#include <string.h>


extern int m_malloc_thread_num;

static inline uint64_t time_of_micros(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}


void *thread_task_exectime_test(void *param) {

    thread_param_t *params = (thread_param_t *)param;
    int64_t max_times = params->max_malloc_times_;
    int64_t seed = params->malloc_size_seed_;
    malloc_type_t type = params->thread_malloc_type;
    pthread_t tid = pthread_self();
    int malloc_id = params->tid;

    if(type == multi_malloc) {
        assert(MAX_TEST_THREAD_NUM <= MAX_MALLOC_THREAD_NUM);
    }

    // set rand seed
    srand((unsigned int)seed);

    void *malloc_ret = NULL;

    uint64_t start_time = time_of_micros();

    size_t idx = 1;
    for(; idx <= max_times; ++idx){
        size_t malloc_size = rand() % 1024;

        if(type == simple_malloc) {
            malloc_ret = s_malloc(malloc_size);
        } else if(type == multi_malloc) {
            malloc_ret = m_malloc(malloc_id, malloc_size);
        }
        if(malloc_ret == NULL) {
            fprintf(stderr, "failed to malloc");
            return NULL;
        }
    }

    uint64_t end_time = time_of_micros();

    printf("tid:%u\n  time passed:%.8lfs\n  each %.4lfus\n", (unsigned int)tid,
                (double)(end_time-start_time)/1000000,
                (double)(end_time-start_time)/idx);
    return malloc_ret;
}

void run_test(int argc, char **argv) {

    uint64_t n;
    uint64_t malloc_times = 1000;
    int thread_num = 1;

    malloc_type_t test_malloc_type = simple_malloc;
    malloc_test_type_t test_bench = exec_time_test;

    char ch_test_type[32];

    for(int i = 0; i < argc; ++i) {
        if (sscanf(argv[i], "--thread_num=%llu", &n)) {
            thread_num = n;
            m_malloc_thread_num = n;
        } else if(sscanf(argv[i], "--malloc_times=%llu", &n)) {
            malloc_times = n;
        } else if(sscanf(argv[i], "--malloc_type=%llu", &n)) {
            test_malloc_type = (n == 0) ? simple_malloc : multi_malloc;
        } else if(strncmp(argv[i], "--test_type=", 12) == 0) {
            strcpy(ch_test_type, argv[i]+12);
        } else if(i > 0) {
            printf("Error parameters: [%s]\n", argv[i]);
            return;
        }
    }
    // parse parameters:
    if (memcmp(ch_test_type, "exectime", 8) == 0) {
        test_bench = exec_time_test;
    }

    assert(thread_num <= MAX_TEST_THREAD_NUM);

    thread_param_t threads_params_[MAX_TEST_THREAD_NUM];
    pthread_t threads_[MAX_TEST_THREAD_NUM];

    // choose one to malloc
    if(test_malloc_type == simple_malloc) {
        s_malloc_init();
    } else if(test_malloc_type == multi_malloc) {
        m_malloc_init();
    }

    for(int i = 0; i < thread_num; ++i) {
        threads_params_[i].max_malloc_times_ = malloc_times;
        threads_params_[i].malloc_size_seed_ = i;
        threads_params_[i].tid = i;
        threads_params_[i].thread_malloc_type = test_malloc_type;
        switch (test_bench)
        {
        case exec_time_test:
            pthread_create(threads_+i, NULL, thread_task_exectime_test, (void*)(threads_params_+i));
            break;
        default:
            break;
        }
    }

    for(int i = 0; i < thread_num; ++i) {
        pthread_join(threads_[i], NULL);
    }

    if(test_malloc_type == simple_malloc) {
        s_malloc_print();
    } else if(test_malloc_type == multi_malloc) {
        m_malloc_print();
    }
}
