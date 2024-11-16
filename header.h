
#ifndef HEADER_H
#define HEADER_H

#include "radixsort.h"

typedef long (*readHexFT)(char*);
extern readHexFT readHexFS[16];
typedef void (*writeHexFT)(char*, long);
extern writeHexFT writeHexFS[16];


unsigned readFile(const char *filename);
ArrayT parseInput(unsigned index);


void writeOutputThread(thread_info* info);
void openOutputMMap(unsigned outputSize);
void closeOutputMMap(unsigned outputSize);
void makeThreadLenCount(unsigned threadNum);
unsigned* getThreadLenCount(unsigned threadIndex);


unsigned long getClockTime();
void random_array(long* arr, unsigned n);
void print_larray(long* arr, unsigned n);
void print_uarray(unsigned* arr, unsigned n);
unsigned array_is_sorted(long* arr, unsigned n);

#endif