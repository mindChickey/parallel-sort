
main: Makefile main.c radixsort.c radixsort.h utils.c hex.o writeOutput.c
	clang main.c radixsort.c utils.c hex.o writeOutput.c -o main -O3

test: test.c radixsort.c radixsort.h utils.c Makefile
	clang test.c radixsort.c utils.c -o test -O3

gen_random: gen_random.c Makefile
	clang gen_random.c -o gen_random -O3

hex.o: hex.cpp Makefile
	clang hex.cpp -O3 -c