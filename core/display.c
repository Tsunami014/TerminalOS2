#include "display.h"

void display(vector list) {
    printf("\033[0;0H\033[K");
    for (int i = 0; i < list.pfVectorSize(&list); i++) {
        printf("%s", list.pfVectorGet(&list, i));
    }
    printf("\033[K");
}
