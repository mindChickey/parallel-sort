
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "radixsort.h"
#include "header.h"

static char* outputMMap;
static unsigned LenCount[10];
static unsigned* threadLenCount;

void makeThreadLenCount(unsigned threadNum){
  threadLenCount = (unsigned*)calloc(10* threadNum, sizeof(unsigned));
}

unsigned* getThreadLenCount(unsigned threadIndex){
  return threadLenCount + threadIndex * 10;
}

void addupLenCount(unsigned threadIndex){
  for(unsigned len = threadIndex; len < 10; len+=context.threadNum){
    for(unsigned i = 0; i < context.threadNum; i++){
      LenCount[len] += threadLenCount[i*10 + len];
    }
  }
}

void dump(char* outputStart, long* nums, unsigned n, unsigned len, struct thread_info* info){
  unsigned threadNum = context.threadNum;
  unsigned n0 = n / threadNum;
  char* str = outputStart + info->index * n0 * len;
  ArrayT arr = getSection(nums, n, threadNum, info->index);

  for(unsigned i = 0; i < arr.length; i++){
    writeHexFS[len-2](str + i * len, arr.data[i]);
    str[i*len + len - 1] = '\n';
  }
}

void writeOutputThread(struct thread_info* info){
  addupLenCount(info->index);
  pthread_barrier_wait(&context.barrier);

  char* outputStart = outputMMap;
  long* nums = info->arr;
  for(unsigned i = 0; i < 10; i++){
    unsigned n = LenCount[i];
    unsigned len = i + 2;

    dump(outputStart, nums, n, len, info);

    outputStart = outputStart + len * n;
    nums = nums + n;
  }
}

void openOutputMMap(unsigned outputSize){
  int fd = open("./sort.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
  ftruncate(fd, outputSize);
  outputMMap = mmap(NULL, outputSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
}

void closeOutputMMap(unsigned outputSize){
  msync(outputMMap, outputSize, MS_SYNC);
  munmap(outputMMap, outputSize);
}