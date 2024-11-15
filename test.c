
#include <stdio.h>
#include <stdlib.h>
#include "radixsort.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <elemNum> <threadNum>\n", argv[0]);
    exit(1);
  }

  unsigned elemNum = atoi(argv[1]);
  unsigned threadNum = atoi(argv[2]);
  
  long* Arr = (long*)malloc(sizeof(long) * (1 << 27));
  long* Brr = (long*)malloc(sizeof(long) * (1 << 27));
  random_array(Arr, elemNum);

  long* order = radix_sort(Arr, Brr, elemNum, threadNum);

  unsigned ok = array_is_sorted(order, elemNum);
  printf("result: %d\n", ok);

  free(Arr);
  free(Brr);
  return 0;
}

// int main1(){
//   Arr[0] = 2;
//   Arr[1] = 1;
//   Arr[2] = 6;
//   Arr[3] = 5;
//   unsigned elemNum = 4;
//   unsigned threadNum = 1;
//   long* order = radix_sort(elemNum, threadNum);
// 
//   unsigned ok = array_is_sorted(order, elemNum);
//   printf("result: %d\n", ok);
// 
//   return 0;
// }