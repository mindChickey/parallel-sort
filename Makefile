
gen_random: gen_random.c
	clang gen_random.c -o gen_random -O3


radixsort_pthreads: radixsort_pthreads.c
	clang radixsort_pthreads.c -o radixsort_pthreads -O3

main: main.cpp
	clang++ main.cpp -o main -O3