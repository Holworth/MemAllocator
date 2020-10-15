#ifndef SIMPLEMALLOC_H_
#define SIMPLEMALLOC_H_
#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <pthread.h>


const char *swp_file_path = "./tmp";
const size_t default_append_size  = 4096;
void *heap_ptr;
unsigned int mem_size = 0;
unsigned int cur_mem_offset = 0;
pthread_mutex_t mem_lock; 

void *s_malloc_init(size_t init_size);
void *s_malloc(size_t size);
void *sys_malloc_alloc(size_t size);

#endif
