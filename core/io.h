#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint64_t KeyPressBitMask[4];
extern KeyPressBitMask* KEYSPRESSED;
bool IsKeyPressed(int code);
typedef struct PressedKeysList {
    int* start;
    int length;
} pressedKeysList;
pressedKeysList* GetPressedKeys();

typedef struct {
    int fd; // File descriptor
    struct libevdev *dev; // libevdev context for device interaction
} KeyboardDevice;

typedef struct KeyEvent keyEvent;
struct KeyEvent {
    int code;
    int state; // 1 = key press, 2 = key held, 0 = key release
    char* name;
    keyEvent* nextEvent;
};
keyEvent* KeyEvent_INIT(int code, int state);

typedef struct KeyEventList {
    keyEvent* start;
    keyEvent* end; // BE CAREFUL; THIS MAY BE NOT NULL WHEN THE LIST IS EMPTY; ONLY CHECK start WHEN CHECKING FOR ELEMENTS IN THE LIST
} keyEventList;
keyEventList* KeyEventList_INIT();
bool KeyEventList_Empty(keyEventList* l);
void KeyEventList_Steal_into(keyEventList* orig, keyEventList* into);
void KeyEventList_Push_Back(keyEventList* l, keyEvent* add);
keyEvent* KeyEventList_Pop_Front(keyEventList* l);
void KeyEventList_Delete(keyEventList* l); // Delete and free contents of list

void IO_init();
void IO_poll(keyEventList* pollInto);
void IO_stop();

#endif
