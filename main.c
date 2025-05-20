#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "core/vector.h"
#include "core/display.h"

struct termios orig_termios;

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(ICRNL | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int mainCode() {
  enableRawMode();
  setvbuf(stdout, NULL, _IONBF, 0); // Disable buffering; we now need to run fflush(stdout); to flush
  printf("\033[2J");
  fflush(stdout);

  VECTOR_INIT(v);

  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    if (c == 0x7F) { // Backspace
        char* it = v.pfVectorGet(&v, v.pfVectorSize(&v)-1);
        v.pfVectorPopBack(&v);
        display(v);
        printf("Deleted: %c\033[K\r\n\033[K", it);
    } else if (iscntrl(c)) {
        display(v);
        printf("Unknown: %d\033[K\r\n", c);
    } else {
        v.pfVectorPushBack(&v, c);
        display(v);
        printf("Added: %d ('%c')\r\n", c, c);
    }
    fflush(stdout);
  }

  disableRawMode();
  return 0;
}

#ifndef WRAPPED_BUILD
int main(void) {
    return mainCode();
}
#endif
