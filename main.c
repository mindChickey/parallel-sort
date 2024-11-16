
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "radixsort.h"
#include "header.h"

#define N 100
//#define N 100000000

void* handle_thread(void* arg){
  struct thread_info* info = (struct thread_info*)arg;
  ArrayT arr = parseInput(info->index);
  info->currentArray = arr;

  radix_sort_thread(arg);
  free(arr.start);
  writeOutputThread(info);
  return NULL;
}

long* handle(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum) {

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
    pthread_create(&thread_handles[i], NULL, handle_thread, (void *)(args+i));
  }

  for (unsigned i = 0; i < threadNum; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  pthread_barrier_destroy(&context.barrier);

  return BITS / BUCKET_BITS % 2 == 0 ? Arr : Brr;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <fileName> <threadNum>\n", argv[0]);
    return 1;
  }

  unsigned threadNum = atoi(argv[2]);
  unsigned fileSize = readFile(argv[1]);

  openOutputMMap(fileSize);

  long* Arr = (long*)malloc(sizeof(long) * (1 << 27));
  long* Brr = (long*)malloc(sizeof(long) * (1 << 27));

  makeThreadLenCount(threadNum);

  long* order = handle(Arr, Brr, N, threadNum);

  closeOutputMMap(fileSize);

  unsigned ok = array_is_sorted(order, N);
  printf("result: %d\n", ok);

  free(Arr);
  free(Brr);

  return 0;
}
