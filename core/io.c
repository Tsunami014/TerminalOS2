#include "io.h"
#include <libevdev/libevdev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_DEVICES 32
static KeyboardDevice devices[MAX_DEVICES];
static int num_devices = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t poll_thread;
static volatile int running = 0;


KeyPressBitMask* KEYSPRESSED = NULL;
bool IsKeyPressed(int code) {
    return (((*KEYSPRESSED)[code >> 6]) & (1ULL << (code & 63))) != 0;
}
void SetKeyPressed(int code) {
    (*KEYSPRESSED)[code >> 6] |= (1ULL << (code & 63));
}
void SetKeyUnpressed(int code) {
    (*KEYSPRESSED)[code >> 6] &= ~(1ULL << (code & 63));
}
pressedKeysList* GetPressedKeys() {
    pressedKeysList* list = malloc(sizeof(pressedKeysList));
    if (list == NULL) {
        return NULL;
    }
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        count += __builtin_popcountll((*KEYSPRESSED)[i]);
    }
    list->length = count;
    int* startptr = malloc(count * sizeof(int));
    if (startptr == NULL) {
        free(list);
        return NULL;
    }
    list->start = startptr;
    unsigned int idx = 0;
    for (unsigned int i = 0; i < 4; ++i) {
        uint64_t bits = (*KEYSPRESSED)[i];
        while (bits) {
            int bit = __builtin_ctzll(bits); // find lowest set bit
            int keycode = (i << 6) | bit;
            startptr[idx++] = keycode;
            bits &= bits - 1;
        }
    }

    return list;
}

keyEvent* KeyEvent_INIT(int code, int state) {
    keyEvent *key = malloc(sizeof(keyEvent));
    if (key == NULL) {
        return NULL;
    }
    key->code = code;
    key->state = state;
    key->name = libevdev_event_code_get_name(state, code);
    key->nextEvent = NULL;
    return key;
}

keyEventList* KeyEventList_INIT() {
    keyEventList *key = malloc(sizeof(keyEventList));
    if (key == NULL) {
        return NULL;
    }
    key->start = NULL;
    key->end = NULL;
    return key;
}
bool KeyEventList_Empty(keyEventList* l) {
    return l->start == NULL;
}
void KeyEventList_Push_Back(keyEventList* l, keyEvent* add) {
    if (l->start != NULL) {
        l->end->nextEvent = add;
        l->end = add;
    } else {
        l->start = add;
        l->end = add;
    }
}
keyEvent* KeyEventList_Pop_Front(keyEventList* l) {
    keyEvent* first = l->start;
    l->start = first->nextEvent;
    return first;
}
void KeyEventList_Steal_into(keyEventList* orig, keyEventList* into) {
    KeyEventList_Delete(into);
    into->start = orig->start;
    into->end = orig->end;
    orig->start = NULL;
    orig->end = NULL;
}
void KeyEventList_Delete(keyEventList* l) {
    while (!KeyEventList_Empty(l)) {
        keyEvent* key = KeyEventList_Pop_Front(l);
        free(key);
    }
}

static keyEventList* KeyEventReaderList = NULL;

static int is_keyboard(struct libevdev *dev) {
    return libevdev_has_event_type(dev, EV_KEY) &&
           libevdev_has_event_code(dev, EV_KEY, KEY_A);
}
static void scan_devices() {
    DIR *dir = opendir("/dev/input");
    if (!dir) {
        perror("Failed to open /dev/input");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "event", 5) != 0) continue;

        char path[256];
        snprintf(path, sizeof(path), "/dev/input/%s", entry->d_name);

        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;

        struct libevdev *dev = NULL;
        if (libevdev_new_from_fd(fd, &dev) >= 0) {
            if (is_keyboard(dev)) {
                if (num_devices < MAX_DEVICES) {
                    devices[num_devices].fd = fd;
                    devices[num_devices].dev = dev;
                    num_devices++;
                } else {
                    libevdev_free(dev);
                    close(fd);
                }
            } else {
                libevdev_free(dev);
                close(fd);
            }
        } else {
            close(fd);
        }
    }

    closedir(dir);
}

static void* polling_thread_func(void* arg) {
    struct pollfd fds[MAX_DEVICES];

    while (running) {
        for (int i = 0; i < num_devices; i++) {
            fds[i].fd = devices[i].fd;
            fds[i].events = POLLIN;
        }

        int rc = poll(fds, num_devices, 100); // 100 ms timeout
        if (rc <= 0) continue;

        for (int i = 0; i < num_devices; i++) {
            if (fds[i].revents & POLLIN) {
                struct input_event ev;
                while (libevdev_next_event(devices[i].dev, LIBEVDEV_READ_FLAG_NORMAL, &ev) == 0) {
                    if (ev.type == EV_KEY) {
                        pthread_mutex_lock(&mutex);
                        if (ev.value == 0) { // Release
                            SetKeyUnpressed(ev.code);
                        } else if (ev.value == 1) { // Press
                            SetKeyPressed(ev.code);
                        }

                        KeyEventList_Push_Back(KeyEventReaderList, KeyEvent_INIT(ev.code, ev.value));
                        pthread_mutex_unlock(&mutex);
                    }
                }
            }
        }
    }

    return NULL;
}


void IO_init() {
    KEYSPRESSED = malloc(sizeof(KeyPressBitMask));
    scan_devices();
    KeyEventReaderList = KeyEventList_INIT();
    running = 1;
    pthread_create(&poll_thread, NULL, polling_thread_func, NULL);
}

void IO_poll(keyEventList* pollInto) {
    pthread_mutex_lock(&mutex);
    KeyEventList_Steal_into(KeyEventReaderList, pollInto);
    pthread_mutex_unlock(&mutex);
}

void IO_stop() {
    running = 0;
    pthread_join(poll_thread, NULL);
    for (int i = 0; i < num_devices; i++) {
        libevdev_free(devices[i].dev);
        close(devices[i].fd);
    }
    num_devices = 0;

    KeyEventList_Delete(KeyEventReaderList);
}
