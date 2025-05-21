#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>

#include "core/io.h"
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
  
  keyEventList* events = KeyEventList_INIT();

  IO_init();

  VECTOR_INIT(v);

  while (true) {
    IO_poll(events);
    bool ran = false;
    while (!KeyEventList_Empty(events)) {
        bool ran = true;
        keyEvent* key = KeyEventList_Pop_Front(events);
        if (key->code == 0x0E) { // Backspace
            char* it = v.pfVectorGet(&v, v.pfVectorSize(&v)-1);
            v.pfVectorPopBack(&v);
            display(v);
            printf("Deleted: %s\033[K\r\n\033[K", it);
        } else {
            v.pfVectorPushBack(&v, key->name);
            display(v);
            printf("Added: %s (%d)\r\n", key->name, key->code);
        }
        free(key);
    }
    if (ran) {
        fflush(stdout);
    }
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
