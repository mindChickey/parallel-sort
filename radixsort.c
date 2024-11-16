
#include <pthread.h>
#include <string.h>
#include "radixsort.h"

struct thread_context context;

ArrayT getSection(long* arr, unsigned elemNum, unsigned secNum, unsigned index){
  ArrayT r;
  unsigned len = elemNum / secNum;
  unsigned start = index * len;

  r.data = arr + start;
  r.length = index == secNum - 1 ? elemNum - start : len;
  return r;
}

unsigned* getCountAddress(unsigned thread_index, long mask){
  return &context.countMatrix[BUCKET_COUNT * thread_index + mask];
}

void count(struct thread_info* info, unsigned bit_pos){
  unsigned* counts = getCountAddress(info->index, 0);
  memset(counts, 0, sizeof(unsigned) * BUCKET_COUNT);
  ArrayT arr = info->currentArray;
  for (long* p = arr.data; p < arr.data + arr.length; p++) {
    long mask = (*p >> bit_pos) & BUCKET_BITMASK;
    counts[mask]++;
  }
}

void getIndices(struct thread_info* info, unsigned indices[BUCKET_COUNT]){
  memset(indices, 0, BUCKET_COUNT * sizeof(unsigned));
  unsigned index = info->index;
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

void moveValues(struct thread_info* info, unsigned bit_pos, unsigned indices[BUCKET_COUNT]){
  /* Move values to correct position. */
  ArrayT arr = info->currentArray;
  for (long* p = arr.data; p < arr.data + arr.length; p++) {
    long mask = (*p >> bit_pos) & BUCKET_BITMASK;
    unsigned index = indices[mask];
    info->brr[index] = *p;
    indices[mask]++;
  }
}

void swapArrBrr(struct thread_info* info){
  long* tmp = info->arr;
  info->arr = info->brr;
  info->brr = tmp;
  info->currentArray = getSection(info->arr, context.elemNum, context.threadNum, info->index);
}

void radix_sort_thread(struct thread_info* info){
  unsigned indices[BUCKET_COUNT];

  for (unsigned bit_pos = 0; bit_pos < BITS; bit_pos += BUCKET_BITS) {
    count(info, bit_pos);

    pthread_barrier_wait(&context.barrier);
    getIndices(info, indices);
    moveValues(info, bit_pos, indices);
    pthread_barrier_wait(&context.barrier);
    swapArrBrr(info);
  }
}

void* standard_radix_sort_thread(void* arg){
  struct thread_info* info = (struct thread_info*)arg;
  info->currentArray = getSection(info->arr, context.elemNum, context.threadNum, info->index);
  radix_sort_thread(info);
  return NULL;
}

long* radix_sort1(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum, void *(*__start_routine)(void *)) {

  unsigned countMatrix[BUCKET_COUNT * threadNum];

  context.elemNum = elemNum;
  context.threadNum = threadNum;
  context.countMatrix = countMatrix;

  pthread_barrier_init(&context.barrier, NULL, threadNum);

  pthread_t thread_handles[threadNum];
  struct thread_info args[threadNum];
  for (unsigned i = 0; i < threadNum; i++) {
    struct thread_info p = {
      .arr = Arr,
      .brr = Brr,
      .index = i,
    };
    args[i] = p;
    pthread_create(&thread_handles[i], NULL, __start_routine, (void *)(args+i));
  }

  for (unsigned i = 0; i < threadNum; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  pthread_barrier_destroy(&context.barrier);

  return args[0].arr;
}

long* radix_sort(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum) {
  return radix_sort1(Arr, Brr, elemNum, threadNum, standard_radix_sort_thread);
}