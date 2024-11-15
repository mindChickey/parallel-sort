
radixsort_pthreads: radixsort_pthreads.c Makefile
	clang radixsort_pthreads.c -o radixsort_pthreads -O3

main: main.cpp Makefile
	clang++ main.cpp -o main -O3

gen_random: gen_random.c Makefile
	clang gen_random.c -o gen_random -O3
