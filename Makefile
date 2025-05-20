# Compile these files too!
CORE = core/display.c core/display.h \
       core/vector.c core/vector.h

# Default target: compile and link the program
main:
	rm -f main
	gcc -Wall -g -static -o main main.c $(CORE)
	chmod +x ./main
init:
	rm -f ./rootfs/init
	gcc -Wall -g -static -o rootfs/init main.c $(CORE)
	chmod +x ./rootfs/init
clean:
	rm -f main
