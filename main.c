
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "radixsort.h"
#include "header.h"

void* handle_thread(void* arg){
  struct thread_info* info = (struct thread_info*)arg;
  ArrayT arr = parseInput(info->index);
  info->currentArray = arr;

  radix_sort_thread(info);
  free(arr.data);
  writeOutputThread(info);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <fileName> <elemNum> <threadNum>\n", argv[0]);
    return 1;
  }

  unsigned elemNum = atoi(argv[2]);
  unsigned threadNum = atoi(argv[3]);
  unsigned fileSize = readFile(argv[1]);

  openOutputMMap(fileSize);

  long* Arr = (long*)malloc(sizeof(long) * (1 << 27));
  long* Brr = (long*)malloc(sizeof(long) * (1 << 27));

  makeThreadLenCount(threadNum);

  long* order = radix_sort1(Arr, Brr, elemNum, threadNum, handle_thread);

  unsigned ok = array_is_sorted(order, elemNum);
  printf("result: %s\n", ok ? "succ" : "fail");

  free(Arr);
  free(Brr);
  closeOutputMMap(fileSize);

  return 0;
}
