#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>

#include "core/io.h"

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
  printf("\033[2J");
  fflush(stdout);
  
  keyEventList* events = KeyEventList_INIT();

  IO_init();

  while (true) {
    IO_poll(events);
    display(events);
    Flush();
  }

  IO_stop();
  KeyEventList_Delete(events);
  disableRawMode();
  return 0;
}

#ifndef WRAPPED_BUILD
int main(void) {
  return mainCode();
}
#endif
