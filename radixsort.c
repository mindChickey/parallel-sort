
#include <pthread.h>
#include <string.h>
#include "radixsort.h"

struct thread_context context;

ArrayT getSection(T* arr, unsigned elemNum, unsigned secNum, unsigned index){
  ArrayT r;
  unsigned len = elemNum / secNum;
  unsigned start = index * len;

  r.data = arr + start;
  r.length = index == secNum - 1 ? elemNum - start : len;
  return r;
}

unsigned* getCountAddress(unsigned thread_index, unsigned mask){
  return &context.countMatrix[BUCKET_COUNT * thread_index + mask];
}

void countBit(thread_info* info, unsigned bit_pos){
  unsigned* counts = getCountAddress(info->index, 0);
  memset(counts, 0, sizeof(unsigned) * BUCKET_COUNT);
  ArrayT arr = info->currentArray;
  for (T* p = arr.data; p < arr.data + arr.length; p++) {
    T mask = (*p >> bit_pos) & BUCKET_BITMASK;
    counts[mask]++;
  }
}

void getIndices(unsigned index, unsigned indices[BUCKET_COUNT]){
  memset(indices, 0, BUCKET_COUNT * sizeof(unsigned));
  for(unsigned i = 0; i < index; i++){
    unsigned* counts = getCountAddress(i, 0);
    for(unsigned k = 0; k < BUCKET_COUNT; k++){
      indices[k] += counts[k];
    }
  }
  for(unsigned i = index; i < context.threadNum; i++){
    unsigned* counts = getCountAddress(i, 0);
    for(unsigned k = 0; k < BUCKET_COUNT - 1; k++){
      indices[k+1] += counts[k];
    }
  }
  for(unsigned k = 0; k < BUCKET_COUNT - 1; k++){
    indices[k+1] += indices[k];
  }
}

void moveValues(thread_info* info, unsigned bit_pos, unsigned indices[BUCKET_COUNT]){
  /* Move values to correct position. */
  ArrayT arr = info->currentArray;
  for (T* p = arr.data; p < arr.data + arr.length; p++) {
    T mask = (*p >> bit_pos) & BUCKET_BITMASK;
    unsigned index = indices[mask];
    info->brr[index] = *p;
    indices[mask]++;
  }
}

void swapArrBrr(thread_info* info){
  T* tmp = info->arr;
  info->arr = info->brr;
  info->brr = tmp;
}

void radix_sort_thread(thread_info* info){
  unsigned indices[BUCKET_COUNT];

  for (unsigned bit_pos = 0; bit_pos < BITS; bit_pos += BUCKET_BITS) {
    countBit(info, bit_pos);

    pthread_barrier_wait(&context.barrier);
    getIndices(info->index, indices);
    moveValues(info, bit_pos, indices);
    pthread_barrier_wait(&context.barrier);
    swapArrBrr(info);
    info->currentArray = getSection(info->arr, context.elemNum, context.threadNum, info->index);
  }
}

void* lsd_radix_sort_thread(void* arg){
  thread_info* info = (thread_info*)arg;
  info->currentArray = getSection(info->arr, context.elemNum, context.threadNum, info->index);
  radix_sort_thread(info);
  return NULL;
}

T* radix_sort1(T* Arr, T* Brr, unsigned elemNum, unsigned threadNum, void *(*__start_routine)(void *)) {

  unsigned countMatrix[BUCKET_COUNT * threadNum];

  context.elemNum = elemNum;
  context.threadNum = threadNum;
  context.countMatrix = countMatrix;

  pthread_barrier_init(&context.barrier, NULL, threadNum);

  pthread_t thread_handles[threadNum];
  thread_info args[threadNum];
  for (unsigned i = 0; i < threadNum; i++) {
    thread_info p = { .arr = Arr, .brr = Brr, .index = i };
    args[i] = p;
    pthread_create(&thread_handles[i], NULL, __start_routine, (void *)(args+i));
  }

  for (unsigned i = 0; i < threadNum; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  pthread_barrier_destroy(&context.barrier);
  return BITS / BUCKET_BITS % 2 == 0 ? Arr : Brr;
}

T* radix_sort(T* Arr, T* Brr, unsigned elemNum, unsigned threadNum) {
  return radix_sort1(Arr, Brr, elemNum, threadNum, lsd_radix_sort_thread);
}