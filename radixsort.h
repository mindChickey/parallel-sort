
#ifndef RADIX_SORT_H
#define RADIX_SORT_H

#include <pthread.h>

#define BITS 40
#define BUCKET_BITS 8
#define BUCKET_COUNT (1lu << BUCKET_BITS)
#define BUCKET_BITMASK (BUCKET_COUNT - 1)

typedef long T;

typedef struct {
  T* data;
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
  T* arr; 
  T* brr;

  unsigned index;     
  ArrayT currentArray;
} thread_info;

ArrayT getSection(T* arr, unsigned elemNum, unsigned secNum, unsigned index);
T* radix_sort(T* Arr, T* Brr, unsigned elemNum, unsigned threadNum);
T* radix_sort1(T* Arr, T* Brr, unsigned elemNum, unsigned threadNum, void *(*__start_routine)(void *));
void radix_sort_thread(thread_info* info);

#endif
