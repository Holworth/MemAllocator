#include "malloctest.h"


const size_t max_thread_num = 10;
static inline uint64_t time_of_micros(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}


void *thread_task(void *param) {
    thread_param_t *params = (thread_param_t *)param;
    int64_t max_times = params->max_malloc_times_;
    int64_t seed = params->malloc_size_seed_;
    pthread_t tid = pthread_self();

    // set rand seed
    srand((unsigned int)seed);

    void *malloc_ret = NULL;

    uint64_t start_time = time_of_micros();

    size_t idx = 1;
    for(; idx <= max_times; ++idx){
        size_t malloc_size = rand() % 1024;
        malloc_ret = s_malloc(malloc_size);
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
    int thread_num = 1;
    uint64_t malloc_times = 1000;

    for(int i = 0; i < argc; ++i) {
        if (sscanf(argv[i], "--thread_num=%llu", &n)) {
            thread_num = n;
        } else if(sscanf(argv[i], "--malloc_times=%llu", &n)) {
            malloc_times = n;
        } else if(i > 0) {
            printf("Error parameters: [%s]\n", argv[i]);
            return;
        }
    }
    thread_param_t threads_params_[MAX_TEST_THREAD_NUM];
    pthread_t threads_[MAX_TEST_THREAD_NUM];


    for(int i = 0; i < thread_num; ++i) {
        threads_params_[i].max_malloc_times_ = malloc_times;
        threads_params_[i].malloc_size_seed_ = i;
        pthread_create(threads_+i, NULL, thread_task, (void*)(threads_params_+i));
    }

    for(int i = 0; i < thread_num; ++i) {
        pthread_join(threads_[i], NULL);
    }
}
