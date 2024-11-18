
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "header.h"

unsigned long getClockTime(){
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  // return ms
  return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

void print_larray(long* arr, unsigned n) {
  for (unsigned i = 0; i < n; i++)
    printf("%lx ", arr[i]);
  printf("\n");
}

void print_uarray(unsigned* arr, unsigned n) {
  for (unsigned i = 0; i < n; i++)
    printf("%u ", arr[i]);
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