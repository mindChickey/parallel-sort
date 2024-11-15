
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BITS 29

long Arr[100000000];
long Brr[100000000];

struct thread_context {
  pthread_barrier_t barrier;
  unsigned threadNum;

  unsigned *nzeros;
  unsigned *nones;
};

struct thread_context context;

struct thread_info {
  long* arr; 
  long* brr;

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

unsigned count0(struct thread_info* p, unsigned bit_pos){
  unsigned count = 0;
  for (unsigned i = p->cur_start; i < p->cur_start + p->cur_len; i++) {
    if (((p->arr[i] >> bit_pos) & 1) == 0) {
      count++;
    }
  }
  return count;
}

void getIndices(struct thread_info* p, unsigned* p0, unsigned* p1){
  /* Get starting indices. */
  unsigned index0 = 0;
  unsigned index1 = p->cur_start;
  for (unsigned i = 0; i < p->index; i++) {
    index0 += context.nzeros[i];
  }
  for (unsigned i = p->index; i < context.threadNum; i++) {
    index1 += context.nzeros[i];
  }
  *p0 = index0;
  *p1 = index1;
}

void moveValues(struct thread_info* p, unsigned bit_pos, unsigned index0, unsigned index1){
  /* Move values to correct position. */
  for (unsigned i = p->cur_start; i < p->cur_start + p->cur_len; i++) {
    if (((p->arr[i] >> bit_pos) & 1) == 0) {
      p->brr[index0++] = p->arr[i];
    } else {
      p->brr[index1++] = p->arr[i];
    }
  }
}

void swapSrcDest(struct thread_info* p){
  long* tmp = p->arr;
  p->arr = p->brr;
  p->brr = tmp;
}

void* radix_sort_thread(void* arg){
  struct thread_info* p = (struct thread_info*)arg;
  unsigned index0, index1;

  for (unsigned bit_pos = 0; bit_pos < BITS; bit_pos++) {
    unsigned num0 = count0(p, bit_pos);
    context.nzeros[p->index] = num0;

    pthread_barrier_wait(&context.barrier);
    getIndices(p, &index0, &index1);
    moveValues(p, bit_pos, index0, index1);
    pthread_barrier_wait(&context.barrier);
    swapSrcDest(p);
  }
  return NULL;
}

long* radix_sort(unsigned n, unsigned t) {

  unsigned nzeros[t];
  unsigned nones[t];
  pthread_t thread_handles[t];
  struct thread_info args[t];

  context.nzeros = nzeros,
  context.nones = nones,
  context.threadNum = t,

  pthread_barrier_init(&context.barrier, NULL, t);

  unsigned len = n / t;
  for (unsigned i = 0; i < t; i++) {
    unsigned start = i * len;
    struct thread_info p = {
      .arr = Arr,
      .brr = Brr,
      .index = i,
      .cur_start = start,
      .cur_len = i == t - 1 ? n - start : len
    };
    args[i] = p;
    pthread_create(&thread_handles[i], NULL, radix_sort_thread, (void *)(args+i));
  }

  for (unsigned i = 0; i < t; i++) {
    pthread_join(thread_handles[i], NULL);
  }

  pthread_barrier_destroy(&context.barrier);

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