
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
  fstat(fileno(file), &st);
  inputSize = st.st_size;
  inputData = malloc(11 * inputSize);
  fread(inputData, inputSize, 1, file);
  fclose(file);
  return inputSize;
}

unsigned parseSection(unsigned threadIndex, long* nums, char* start, char* end){
  unsigned n = 0;
  unsigned* count = getThreadLenCount(threadIndex);
  while(start < end){
    char* end = strchr(start, '\n');
    unsigned len_1 = end-start-1;
    count[len_1]++;

    unsigned long hex = readHexFS[len_1](start);
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
  unsigned n = parseSection(index, nums, start, end);

  ArrayT arr = {nums, n};
  return arr;
}