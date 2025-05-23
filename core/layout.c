#include "layout.h"

void layout_init(layout* l, int width, int height) {
    l->width = width;
    l->height = height;
    l->firstRow = NULL;
}
