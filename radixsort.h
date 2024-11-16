
#ifndef RADIX_SORT_H
#define RADIX_SORT_H

#include <pthread.h>

#define BITS 40
#define BUCKET_BITS 8
#define BUCKET_COUNT (1lu << BUCKET_BITS)
#define BUCKET_BITMASK (BUCKET_COUNT - 1)

typedef struct {
  long* data;
  unsigned length;
} ArrayT;

struct thread_context {
  pthread_barrier_t barrier;
  unsigned elemNum;
  unsigned threadNum;

  unsigned* countMatrix;
};

extern struct thread_context context;

typedef struct {
  long* arr; 
  long* brr;

  unsigned index;     
  ArrayT currentArray;
} thread_info;

ArrayT getSection(long* arr, unsigned elemNum, unsigned secNum, unsigned index);
long* radix_sort(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum);
long* radix_sort1(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum, void *(*__start_routine)(void *));
void radix_sort_thread(thread_info* info);

#endif
