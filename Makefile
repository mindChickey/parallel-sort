
test: test.c radixsort.c Makefile
	clang test.c radixsort.c -o test -O3

main: main.cpp Makefile
	clang++ main.cpp -o main -O3

gen_random: gen_random.c Makefile
	clang gen_random.c -o gen_random -O3
