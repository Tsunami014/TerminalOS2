#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int fd; // File descriptor
    struct libevdev *dev; // libevdev context for device interaction
} KeyboardDevice;

typedef struct KeyEvent keyEvent;
struct KeyEvent {
    int code;
    int value; // 1 = key press, 0 = key release
    char* name;
    keyEvent* nextEvent;
};
keyEvent* KeyEvent_INIT(int code, int value);

typedef struct KeyEventList {
    keyEvent* start;
    keyEvent* end; // BE CAREFUL; THIS MAY BE NOT NULL WHEN THE LIST IS EMPTY; ONLY CHECK start WHEN CHECKING FOR ELEMENTS IN THE LIST
} keyEventList;
keyEventList* KeyEventList_INIT();
bool KeyEventList_Empty(keyEventList* l);
void KeyEventList_Steal_into(keyEventList* orig, keyEventList* into);
void KeyEventList_Push_Back(keyEventList* l, keyEvent* add);
keyEvent* KeyEventList_Pop_Front(keyEventList* l);
void KeyEventList_Delete(keyEventList* l); // Delete and free list and all contents

void IO_init();
void IO_poll(keyEventList* pollInto);
void IO_stop();

#endif
