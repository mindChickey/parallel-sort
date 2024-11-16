
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "radixsort.h"
#include "header.h"

char* outputMMap;
unsigned LenCount[10];
unsigned* threadLenCount;

void makeThreadLenCount(unsigned threadNum){
  threadLenCount = (unsigned*)calloc(10* threadNum, sizeof(unsigned));
}

void makeLenCount(unsigned index){
  for(unsigned len = index; len < 10; len+=context.threadNum){
    for(unsigned i = 0; i < context.threadNum; i++){
      LenCount[len] += threadLenCount[i*10 + len];
    }
  }
}

void dump(char* outputStart, long* nums, unsigned n, unsigned len, struct thread_info* info){
  unsigned threadNum = context.threadNum;
  unsigned n0 = n / threadNum;
  char* str = outputStart + info->index * n0 * len;
  long* nums1 = nums + info->index * n0;
  unsigned n1 = info->index == threadNum - 1 ? nums + n - nums1 : n0;

  for(unsigned i = 0; i < n1; i++){
    writeHexFS[len-2](str + i * len, nums1[i]);
    str[i*len + len - 1] = '\n';
  }
}

void writeOutputThread(struct thread_info* info){
  makeLenCount(info->index);
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