
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "header.h"

static char* inputData;
static unsigned inputSize;

unsigned readFile(const char *filename) {
  FILE* file = fopen(filename, "r");
  struct stat st;
  int fd = file->_fileno;
  fstat(fd, &st);
  inputSize = st.st_size;
  inputData = mmap(NULL, inputSize, PROT_READ, MAP_PRIVATE, fd, 0);
  fclose(file);
  return inputSize;
}

char* parseANum(unsigned* countArr, char* start, unsigned long* hex){
  char* end0 = strchr(start, '\n');
  unsigned len_1 = end0-start-1;
  countArr[len_1]++;
  *hex = readHexFS[len_1](start);
  return end0 + 1;
}

unsigned parseSection(unsigned threadIndex, long* nums, char* start, char* end){
  unsigned n = 0;
  unsigned* countArr = getThreadLenCount(threadIndex);
  while(start < end){
    unsigned long hex;
    start = parseANum(countArr, start, &hex);
    nums[n] = hex;
    n++;
  }
  return n;
}

ArrayT parseInput(unsigned index) {
  unsigned len0 = inputSize / context.threadNum;
  char* start = index == 0 ? inputData : strchr(inputData + index * len0, '\n') + 1;
  char* end = index == context.threadNum - 1 ? inputData + inputSize : strchr(inputData + (index+1) * len0, '\n') + 1;

  long* nums = (long*)malloc((end-start) / 2 * sizeof(long));
  unsigned n = parseSection(index, nums, start, end);

  ArrayT arr = {nums, n};
  return arr;
}