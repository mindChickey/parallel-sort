
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

void countBit(thread_info* info, unsigned bit_pos);
void getIndices(unsigned index, unsigned indices[BUCKET_COUNT]);
void moveValues(thread_info* info, unsigned bit_pos, unsigned indices[BUCKET_COUNT]);
void swapArrBrr(thread_info* info);
T* radix_sort1(T* Arr, T* Brr, unsigned elemNum, unsigned threadNum, void *(*__start_routine)(void *));

T* radix_sort(T* Arr, T* Brr, unsigned elemNum, unsigned threadNum);
void radix_sort_thread(thread_info* info);
void radix_sort_msd(thread_info* info, unsigned bit_pos);

T* msd_radix_sort(T* Arr, T* Brr, unsigned elemNum, unsigned threadNum);
void* msd_radix_sort_thread(void* arg);

#endif
