
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include "radixsort.h"

void rec(thread_info* info, unsigned elemNum, unsigned bit_pos){
  info->currentArray = getSection(info->arr, elemNum, context.threadNum, info->index);
  radix_sort_msd(info, bit_pos - BUCKET_BITS);
}

void radix_sort_msd(thread_info* info, unsigned bit_pos){
  unsigned indices[BUCKET_COUNT];
  countBit(info, bit_pos - BUCKET_BITS);
  pthread_barrier_wait(&context.barrier);
  getIndices(info->index, indices);
  moveValues(info, bit_pos - BUCKET_BITS, indices);

  pthread_barrier_wait(&context.barrier);

  if(bit_pos > BUCKET_BITS) {
    thread_info info1 = *info;
    getIndices(context.threadNum, indices);

    info1.arr = info->brr;
    info1.brr = info->arr;
    rec(&info1, indices[0], bit_pos);

    for(unsigned k = 1; k < BUCKET_COUNT; k++){
      unsigned elemNum1 = indices[k] - indices[k-1];
      if(elemNum1 > 0) {
        info1.arr = info->brr + indices[k-1];
        info1.brr = info->arr + indices[k-1];
        rec(&info1, elemNum1, bit_pos);
      }
    }
  }
}

void* msd_radix_sort_thread(void* arg){
  thread_info* info = (thread_info*)arg;
  info->currentArray = getSection(info->arr, context.elemNum, context.threadNum, info->index);
  radix_sort_msd(info, BITS);
  return NULL;
}

T* msd_radix_sort(T* Arr, T* Brr, unsigned elemNum, unsigned threadNum) {
  // TODO: some bug in multi thread
  return radix_sort1(Arr, Brr, elemNum, threadNum, msd_radix_sort_thread);
}