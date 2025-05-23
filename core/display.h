#ifndef CORE_DISPLAY_H
#define CORE_DISPLAY_H

#include "io.h"

void Print(const char *str);
void PrintF(const char *fmt, ...);
void PrintC(const char c);
void Flush();

void display(keyEventList* evs);

#endif