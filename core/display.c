#include "display.h"
#include "io.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

static char buf[1024];
static int len = 0;

void Print(const char *str) {
    while (*str && len < (int)sizeof(buf)) {
        buf[len++] = *str++;
    }
}

void PrintF(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(buf + len, sizeof(buf) - len, fmt, args);
    if (n > 0) {
        // Ensure we don't overflow the buffer
        if ((len + n) < (int)sizeof(buf))
            len += n;
        else
            len = sizeof(buf) - 1;
    }
    va_end(args);
}

void PrintC(const char c) {
    if (len < (int)sizeof(buf)) {
        buf[len++] = c;
    }
}

void Flush() {
    if (len > 0) {
        write(STDOUT_FILENO, buf, len);
        len = 0;
    }
}

void display(keyEventList* evs) {
    if (!KeyEventList_Empty(evs)) {
        Print("\033[0;0H");
        for (int i = 0; i < 4; ++i) {
            uint64_t val = (*KEYSPRESSED)[i];
            for (int bit = 63; bit >= 0; --bit) {
                PrintC((val & (1ULL << bit)) ? '1' : '0');
            }
        }


        /*pressedKeysList* keys = GetPressedKeys();
        for (int i = 0; i < keys->length; i++) {
            printf("%s", libevdev_event_code_get_name(1, keys->start[i]));
        }*/
        Print("\033[K");
        //fflush(stdout);
    }
}
