
#include <stdio.h>
#include <stdlib.h>
#include "radixsort.h"
#include "header.h"

long nums0[] = {0x17, 0x16, 0x9, 0x21}; 
long nums1[] = {0x3, 0x1, 0x2, 0x8}; 
ArrayT arrs[2] = {{nums0, 4}, {nums1, 4}};

void* merge_radix_sort_thread(void* arg){
  thread_info* info = (thread_info*)arg;
  info->currentArray = arrs[info->index];
  radix_sort_thread(info);
  return NULL;
}

int main0(){
  long Arr[8];
  long Brr[8];
  long* order = radix_sort1(Arr, Brr, 8, 2, merge_radix_sort_thread);
  print_larray(order, 8);
  return 0;
}

void test1(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum){
  random_array(Arr, elemNum);
  unsigned long t0 = getClockTime();
  long* order = radix_sort(Arr, Brr, elemNum, threadNum);
  unsigned long t1 = getClockTime();
  printf("time %lu\n", t1 -t0);
  unsigned ok = array_is_sorted(order, elemNum);
  printf("result: %s\n", ok ? "succ" : "fail");
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <elemNum> <threadNum>\n", argv[0]);
    exit(1);
  }
  unsigned elemNum = atoi(argv[1]);
  unsigned threadNum = atoi(argv[2]);
  long* Arr = malloc(elemNum * sizeof(long));
  long* Brr = malloc(elemNum * sizeof(long));
  test1(Arr, Brr, elemNum, threadNum);
  free(Arr);
  free(Brr);
  return 0;
}
