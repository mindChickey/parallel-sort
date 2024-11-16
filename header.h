
#ifndef HEADER_H
#define HEADER_H

#include "radixsort.h"

extern unsigned* threadLenCount;
typedef void (*writeHexFT)(char*, long);

extern writeHexFT writeHexFS[16];

extern unsigned LenCount[10];

void writeOutputThread(struct thread_info* info);
void openOutputMMap(unsigned outputSize);
void closeOutputMMap(unsigned outputSize);
 
#endif