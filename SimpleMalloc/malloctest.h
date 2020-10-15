#ifndef MALLOCTEST_H_
#define MALLOCTEST_H_

#include "simplemalloc.h"
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
//#define SINGLETHREAD_TEST
#define MULTITHREAD_TEST

void singlethread_test();
void multithread_test();
void singlethread_malloc();

#endif