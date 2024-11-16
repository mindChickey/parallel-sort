
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

void test0(long* Arr, long* Brr){
  long nums0[] = {7, 6, 9}; 
  long nums1[] = {3, 1, 2, 8}; 

  ArrayT arrs[2] = {{nums0, 3}, {nums1, 4}};
  long* order = radix_sort1(Arr, Brr, arrs, 7, 2);
  print_larray(order, 7);
}

void test1(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum){
  random_array(Arr, elemNum);
  long* order = radix_sort(Arr, Brr, elemNum, threadNum);
  unsigned ok = array_is_sorted(order, elemNum);
  printf("result: %d\n", ok);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <elemNum> <threadNum>\n", argv[0]);
    exit(1);
  }

  unsigned elemNum = atoi(argv[1]);
  unsigned threadNum = atoi(argv[2]);
  
  long* Arr = (long*)malloc(sizeof(long) * (1 << 27));
  long* Brr = (long*)malloc(sizeof(long) * (1 << 27));

  test1(Arr, Brr, elemNum, threadNum);

  free(Arr);
  free(Brr);
  return 0;
}
