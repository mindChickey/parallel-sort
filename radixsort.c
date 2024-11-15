
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "radixsort.h"

struct thread_context context;

unsigned* getCountAddress(unsigned thread_index, long mask){
  return &context.countMatrix[BUCKET_COUNT * thread_index + mask];
}

void count(struct thread_info* info, unsigned bit_pos){
  unsigned* counts = getCountAddress(info->index, 0);
  memset(counts, 0, sizeof(unsigned) * BUCKET_COUNT);
  for (unsigned i = info->cur_start; i < info->cur_start + info->cur_len; i++) {
    long mask = (info->arr[i] >> bit_pos) & BUCKET_BITMASK;
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
  for (unsigned i = info->cur_start; i < info->cur_start + info->cur_len; i++) {
    long mask = (info->arr[i] >> bit_pos) & BUCKET_BITMASK;
    unsigned index = indices[mask];
    info->brr[index] = info->arr[i];
    indices[mask]++;
  }
}

void swapSrcDest(struct thread_info* info){
  long* tmp = info->arr;
  info->arr = info->brr;
  info->brr = tmp;
}

void* radix_sort_thread(void* arg){
  struct thread_info* info = (struct thread_info*)arg;
  unsigned indices[BUCKET_COUNT];

  for (unsigned bit_pos = 0; bit_pos < BITS; bit_pos += BUCKET_BITS) {
    count(info, bit_pos);

    pthread_barrier_wait(&context.barrier);
    getIndices(info, indices);
    moveValues(info, bit_pos, indices);
    pthread_barrier_wait(&context.barrier);
    swapSrcDest(info);
  }
  return NULL;
}

long* radix_sort(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum) {

  unsigned countMatrix[BUCKET_COUNT * threadNum];

  context.threadNum = threadNum;
  context.countMatrix = countMatrix;

  pthread_barrier_init(&context.barrier, NULL, threadNum);

  pthread_t thread_handles[threadNum];
  struct thread_info args[threadNum];
  unsigned len = elemNum / threadNum;
  for (unsigned i = 0; i < threadNum; i++) {
    unsigned start = i * len;
    struct thread_info p = {
      .arr = Arr,
      .brr = Brr,
      .index = i,
      .cur_start = start,
      .cur_len = i == threadNum - 1 ? elemNum - start : len
    };
    args[i] = p;
    pthread_create(&thread_handles[i], NULL, radix_sort_thread, (void *)(args+i));
  }

  for (unsigned i = 0; i < threadNum; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  pthread_barrier_destroy(&context.barrier);

  return BITS / BUCKET_BITS % 2 == 0 ? Arr : Brr;
}
