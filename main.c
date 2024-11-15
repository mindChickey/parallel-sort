
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "radixsort.h"

#define N 100
// #define N 100000000
#define MAX_NUM_LEN 11  // 最大数字长度(10位)加换行符(1位)

char inputData[N * MAX_NUM_LEN];
unsigned inputSize;

typedef long (*readHexFT)(char*);
extern readHexFT readHexFS[16];

void readFile(const char *filename) {
  FILE* file = fopen(filename, "r");
  struct stat st;
  fstat(fileno(file), &st);
  inputSize = st.st_size;
  fread(inputData, inputSize, 1, file);
  fclose(file);
}

unsigned parseSection(long* nums, char* start, char* end){
  unsigned n = 0;
  while(start < end){
    char* end = strchr(start, '\n');
    unsigned long hex = readHexFS[end-start-1](start);
    nums[n] = hex;
    n++;
    start = end + 1;
  }
  return n;
}

ArrayT parseInput(unsigned index) {
  unsigned len0 = inputSize / context.threadNum;
  char* start = index == 0 ? inputData : strchr(inputData + index * len0, '\n') + 1;
  char* end = index == context.threadNum - 1 ? inputData + inputSize : strchr(inputData + (index+1) * len0, '\n') + 1;

  long* nums = (long*)malloc((end-start) / 2 * sizeof(long));
  unsigned n = parseSection(nums, start, end);

  ArrayT arr = {nums, n};
  return arr;
}

void* handle_thread(void* arg){
  struct thread_info* info = (struct thread_info*)arg;
  ArrayT arr = parseInput(info->index);
  info->currentArray = arr;

  radix_sort_thread(arg);

  free(arr.start);
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
  readFile(argv[1]);

  long* Arr = (long*)malloc(sizeof(long) * (1 << 27));
  long* Brr = (long*)malloc(sizeof(long) * (1 << 27));

  long* order = handle(Arr, Brr, N, threadNum);

  unsigned ok = array_is_sorted(order, N);
  printf("result: %d\n", ok);

  free(Arr);
  free(Brr);

  return 0;
}
