
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "radixsort.h"

unsigned long getClockTime(){
  struct timespec t;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
  // return ms
  return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

void print_larray(long* arr, unsigned n) {
  for (unsigned i = 0; i < n; i++)
    printf("%lx \n", arr[i]);
  printf("\n");
}

void print_uarray(unsigned* arr, unsigned n) {
  for (unsigned i = 0; i < n; i++)
    printf("%u \n", arr[i]);
  printf("\n");
}

void random_array(long* arr, unsigned n) {
  srandom(time(0));
  for (unsigned i = 0; i < n; i++) {
    arr[i] = random() & ((1l << BITS) - 1);
  }
}

unsigned array_is_sorted(long* arr, unsigned n) {
  for (unsigned i = 1; i < n; i++)
    if (arr[i - 1] > arr[i])
      return 0;
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "need elemNum and threadNum\n");
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