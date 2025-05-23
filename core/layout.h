#ifndef LAYOUT_H
#define LAYOUT_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Cell {
    int width;
    int* height;
} cell;
typedef struct Row row;
struct Row {
    row* nextRow;
    cell* firstElm;
    int length;
    int height;
    int* maxWid;
};
typedef struct Layout {
    int width;
    int height;
    row* firstRow;
} layout;
void layout_init(layout* l, int width, int height);
#define LAYOUT_INIT(lay, width, height) layout lay; layout_init(&lay, width, height)

#endif
