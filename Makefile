# Compile these files too!
CORE = core/display.c core/display.h \
	   core/io.c core/io.h \
	   core/layout.c core/layout.h

CFLAGS = -Wall -g -static $(shell pkg-config --cflags libevdev)
LDFLAGS = $(shell pkg-config --libs libevdev)

# — Create the main file —————————————————————————————————————————————
main: Makefile main.c $(CORE)
	rm -f main
	gcc $(CFLAGS) -o main main.c $(CORE) $(LDFLAGS)
	chmod +x ./main

# — Create the init file —————————————————————————————————————————————
init: Makefile init.c main.c $(CORE)
	rm -f ./rootfs/init
	gcc $(CFLAGS) -DWRAPPED_BUILD -o rootfs/init init.c main.c $(CORE) $(LDFLAGS)
	chmod +x ./rootfs/init

# — Convenience: run under kitty and pause at end ————————————————————
test: main
	@kitty bash -c './main; echo; read -p "Program finished! Press enter to continue."'

# — Clean out everything —————————————————————————————————————————————
clean:
	rm -f main
	rm -f ./rootfs/init
