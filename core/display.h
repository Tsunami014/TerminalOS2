#ifndef DISPLAY_H
#define DISPLAY_H

#include "io.h"

void Print(const char *str);
void PrintF(const char *fmt, ...);
void PrintC(const char c);
void Flush();

void display(keyEventList* evs);

#endif