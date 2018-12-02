CC = g++

build: jobshop2.cpp
	$(CC) -std=c++17 jobshop.cpp -o jobshop.o

clean:
	rm -f jobshop
	rm -rf output/*

rune: jobshop.o
	./jobshop.o b input/beasley/abz5.txt

runh: jobshop.o
	./jobshop.o t input/tailard/tai75.txt

runall: jobshop.o run.sh
	./run.sh
