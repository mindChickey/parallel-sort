
#ifndef RADIX_SORT_H
#define RADIX_SORT_H

#include <pthread.h>

#define BITS 32
#define BUCKET_BITS 8
#define BUCKET_COUNT (1lu << BUCKET_BITS)
#define BUCKET_BITMASK (BUCKET_COUNT - 1)

unsigned long getClockTime();
void random_array(long* arr, unsigned n);
void print_larray(long* arr, unsigned n);
void print_uarray(unsigned* arr, unsigned n);
unsigned array_is_sorted(long* arr, unsigned n);

struct thread_context {
  pthread_barrier_t barrier;
  unsigned elemNum;
  unsigned threadNum;

  unsigned* countMatrix;
};

extern struct thread_context context;

typedef struct {
  long* start;
  unsigned length;
} ArrayT;

struct thread_info {
  long* arr; 
  long* brr;

  unsigned index;     
  ArrayT currentArray;
};

long* radix_sort1(long* Arr, long* Brr, ArrayT* inputs, unsigned elemNum, unsigned threadNum);
long* radix_sort(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum);
void* radix_sort_thread(void* arg);

#endif
