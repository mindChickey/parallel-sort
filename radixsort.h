
#ifndef RADIX_SORT_H
#define RADIX_SORT_H

#define BITS 32
#define BUCKET_BITS 8
#define BUCKET_COUNT (1lu << BUCKET_BITS)
#define BUCKET_BITMASK (BUCKET_COUNT - 1)

unsigned long getClockTime();

long* radix_sort(long* Arr, long* Brr, unsigned elemNum, unsigned threadNum);

#endif
