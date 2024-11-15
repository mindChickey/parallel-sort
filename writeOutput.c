
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "radixsort.h"

extern unsigned* threadLenCount;
unsigned LenCount[10];

void makeLenCount(unsigned index){
  for(unsigned len = index; len < 10; len+=context.threadNum){
    for(unsigned i = 0; i < context.threadNum; i++){
      LenCount[len] += threadLenCount[i*10 + len];
    }
  }
}

void writeOutput(struct thread_info* info){
  makeLenCount(info->index);

}