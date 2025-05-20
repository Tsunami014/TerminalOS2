# Compile these files too!
CORE = core/display.c core/display.h \
       core/vector.c core/vector.h

# Default target: compile and link the program
all:
	rm -f init
	gcc -Wall -g -static -o init main.c $(CORE)
