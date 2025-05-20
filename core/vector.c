#include <stdio.h>

// Thanks so much to https://aticleworld.com/implement-vector-in-c/ where the original code came from!

#define VECTOR_INIT_CAPACITY 6
#define UNDEFINE -1
#define SUCCESS 0

typedef struct VectorList {
    void **items;
    int capacity;
    int total;
} VectorList;
//structure contain the function pointer
typedef struct Vector vector;
struct Vector
{
    VectorList vecList;
//function pointers
    int (*pfVectorSize)(vector *);
    int (*pfVectorResize)(vector *, int);
    int (*pfVectorPushBack)(vector *, void *);
    void *(*pfVectorPopBack)(vector *);
    int (*pfVectorSet)(vector *, int, void *);
    void *(*pfVectorGet)(vector *, int);
    int (*pfVectorDelete)(vector *, int);
    int (*pfVectorFree)(vector *);
};

int vectorSize(vector *v) {
    int totalCount = UNDEFINE;
    if (v) {
        totalCount = v->vecList.total;
    }
    return totalCount;
}
int vectorResize(vector *v, int capacity) {
    int status = UNDEFINE;
    if (v) {
        void **items = realloc(v->vecList.items, sizeof(void *) * capacity);
        if (items) {
            v->vecList.items = items;
            v->vecList.capacity = capacity;
            status = SUCCESS;
        }
    }
    return status;
}
int vectorPushBack(vector *v, void *item) {
    int status = UNDEFINE;
    if (v) {
        if (v->vecList.capacity == v->vecList.total) {
            status = vectorResize(v, v->vecList.capacity * 2);
            if (status != UNDEFINE) {
                v->vecList.items[v->vecList.total++] = item;
            }
        } else {
            v->vecList.items[v->vecList.total++] = item;
            status = SUCCESS;
        }
    }
    return status;
}
void *vectorPopBack(vector *v) {
    void *item = NULL;
    if (v && v->vecList.total > 0) {
        item = v->vecList.items[v->vecList.total - 1];
        v->vecList.items[v->vecList.total - 1] = NULL;
        v->vecList.total--;

        if (v->vecList.total > 0 && v->vecList.total == v->vecList.capacity / 4) {
            v->pfVectorResize(v, v->vecList.capacity / 2);
        }
    }
    return item;
}
int vectorSet(vector *v, int index, void *item) {
    int status = UNDEFINE;
    if (v) {
        if ((index >= 0) && (index < v->vecList.total)) {
            v->vecList.items[index] = item;
            status = SUCCESS;
        }
    }
    return status;
}
void *vectorGet(vector *v, int index) {
    void *readData = NULL;
    if (v) {
        if ((index >= 0) && (index < v->vecList.total)) {
            readData = v->vecList.items[index];
        }
    }
    return readData;
}
int vectorDelete(vector *v, int index) {
    int status = UNDEFINE;
    int i = 0;
    if (v) {
        if ((index < 0) || (index >= v->vecList.total)) {
            return status;
        }
        v->vecList.items[index] = NULL;
        for (i = index; (i < v->vecList.total - 1); ++i) {
            v->vecList.items[i] = v->vecList.items[i + 1];
            v->vecList.items[i + 1] = NULL;
        }
        v->vecList.total--;
        if ((v->vecList.total > 0) && ((v->vecList.total) == (v->vecList.capacity / 4))) {
            vectorResize(v, v->vecList.capacity / 2);
        }
        status = SUCCESS;
    }
    return status;
}
int vectorFree(vector *v) {
    int status = UNDEFINE;
    if (v) {
        free(v->vecList.items);
        v->vecList.items = NULL;
        status = SUCCESS;
    }
    return status;
}
void vector_init(vector *v) {
    //init function pointers
    v->pfVectorSize = vectorSize;
    v->pfVectorResize = vectorResize;
    v->pfVectorPushBack = vectorPushBack;
    v->pfVectorPopBack = vectorPopBack;
    v->pfVectorSet = vectorSet;
    v->pfVectorGet = vectorGet;
    v->pfVectorFree = vectorFree;
    v->pfVectorDelete = vectorDelete;
    //initialize the capacity and allocate the memory
    v->vecList.capacity = VECTOR_INIT_CAPACITY;
    v->vecList.total = 0;
    v->vecList.items = malloc(sizeof(void *) * v->vecList.capacity);
}
