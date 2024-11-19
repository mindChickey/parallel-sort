
main: Makefile main.c radixsort.c radixsort.h header.h utils.c hex.cpp writeOutput.c readInput.c
	clang main.c radixsort.c utils.c hex.cpp writeOutput.c readInput.c -o main -O3 -flto

sort_test: sort_test.c msd_radixsort.c radixsort.c radixsort.h utils.c Makefile
	clang sort_test.c msd_radixsort.c radixsort.c utils.c -o sort_test -O3 -flto

gen_random: gen_random.c Makefile
	clang gen_random.c -o gen_random -O3 -flto

clean:
	rm main test gen_random