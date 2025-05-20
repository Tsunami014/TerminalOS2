# Default target: compile and link the program
all:
	rm -f init
	gcc -Wall -g -static -o init main.c
