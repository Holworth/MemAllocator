#ifndef SIMPLEMALLOC_H_
#define SIMPLEMALLOC_H_
#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#define max(a, b) ((a) > (b)) ? (a) : (b)

void *s_malloc_init();
void *s_malloc(size_t size);
void *sys_malloc_alloc(size_t size);

#endif
