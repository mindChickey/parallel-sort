
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BITS 29
pthread_barrier_t barrier;

long Arr[100000000];
long Brr[100000000];

struct rs_args {
  long* arr; 
  long* brr;
  unsigned elemNum;       
  unsigned threadNum; 

  unsigned *nzeros;   
  unsigned *nones;    

  unsigned index;     
  unsigned cur_start;
  unsigned cur_len;
};

void print_array(long* arr, unsigned n) {
  for (unsigned i = 0; i < n; i++)
    printf("%ld \n", arr[i]);
  printf("\n");
}

void random_array(long* arr, unsigned n) {
  for (unsigned i = 0; i < n; i++) {
    arr[i] = (unsigned)lrand48() & (unsigned)((1 << BITS) - 1);
  }
}

unsigned array_is_sorted(long* arr, unsigned n) {
  for (unsigned i = 1; i < n; i++)
    if (arr[i - 1] > arr[i])
      return 0;
  return 1;
}

unsigned count0(struct rs_args* p, unsigned bit_pos){
  unsigned count = 0;
  for (unsigned i = p->cur_start; i < p->cur_start + p->cur_len; i++) {
    if (((p->arr[i] >> bit_pos) & 1) == 0) {
      count++;
    }
  }
  return count;
}

void getIndices(struct rs_args* p, unsigned* p0, unsigned* p1){
  /* Get starting indices. */
  unsigned index0 = 0;
  unsigned index1 = p->cur_start;
  for (unsigned i = 0; i < p->index; i++) {
    index0 += p->nzeros[i];
  }
  for (unsigned i = p->index; i < p->threadNum; i++) {
    index1 += p->nzeros[i];
  }
  *p0 = index0;
  *p1 = index1;
}

void moveValues(struct rs_args* p, unsigned bit_pos, unsigned index0, unsigned index1){
  /* Move values to correct position. */
  for (unsigned i = p->cur_start; i < p->cur_start + p->cur_len; i++) {
    if (((p->arr[i] >> bit_pos) & 1) == 0) {
      p->brr[index0++] = p->arr[i];
    } else {
      p->brr[index1++] = p->arr[i];
    }
  }
}

void swapSrcDest(struct rs_args* p){
  long* tmp = p->arr;
  p->arr = p->brr;
  p->brr = tmp;
}

void* radix_sort_thread(void* arg){
  struct rs_args* p = (struct rs_args*)arg;
  unsigned index0, index1;

  for (unsigned bit_pos = 0; bit_pos < BITS; bit_pos++) {
    unsigned num0 = count0(p, bit_pos);
    p->nzeros[p->index] = num0;

    pthread_barrier_wait(&barrier);
    getIndices(p, &index0, &index1);
    moveValues(p, bit_pos, index0, index1);
    pthread_barrier_wait(&barrier);
    swapSrcDest(p);
  }
  return NULL;
}

long* radix_sort(unsigned n, unsigned t) {

  unsigned nzeros[t];
  unsigned nones[t];
  pthread_t thread_handles[t];
  struct rs_args args[t];

  pthread_barrier_init(&barrier, NULL, t);

  unsigned len = n / t;
  for (unsigned i = 0; i < t; i++) {
    unsigned start = i * len;
    struct rs_args p = {
      .index = i,
      .arr = Arr,
      .brr = Brr,
      .elemNum = n,
      .nzeros = nzeros,
      .nones = nones,
      .threadNum = t,
      .cur_start = start,
      .cur_len = i == t - 1 ? n - start : len
    };
    args[i] = p;
    pthread_create(&thread_handles[i], NULL, radix_sort_thread, (void *)(args+i));
  }

  for (unsigned i = 0; i < t; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  pthread_barrier_destroy(&barrier);

  return BITS % 2 == 0 ? Arr : Brr;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "need elemNum and threadNum\n");
    exit(1);
  }

  unsigned elemNum = atoi(argv[1]);
  unsigned threadNum = atoi(argv[2]);
  
  random_array(Arr, elemNum);

  long* order = radix_sort(elemNum, threadNum);

  unsigned ok = array_is_sorted(order, elemNum);
  printf("result: %d\n", ok);

  return 0;
}