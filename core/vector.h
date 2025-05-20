#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>

#define VECTOR_INIT_CAPACITY 6
#define UNDEFINE -1
#define SUCCESS 0

#define VECTOR_INIT(vec) vector vec; vector_init(&vec)

typedef struct VectorList {
    void **items;
    int capacity;
    int total;
} VectorList;

typedef struct Vector vector;
struct Vector {
    VectorList vecList;

    // Function pointers
    int (*pfVectorSize)(vector *);
    int (*pfVectorResize)(vector *, int);
    int (*pfVectorPushBack)(vector *, void *);
    void *(*pfVectorPopBack)(vector *);
    int (*pfVectorSet)(vector *, int, void *);
    void *(*pfVectorGet)(vector *, int);
    int (*pfVectorDelete)(vector *, int);
    int (*pfVectorFree)(vector *);
};

// Function declarations
int vectorSize(vector *v);
int vectorResize(vector *v, int capacity);
int vectorPushBack(vector *v, void *item);
int vectorSet(vector *v, int index, void *item);
void *vectorGet(vector *v, int index);
int vectorDelete(vector *v, int index);
int vectorFree(vector *v);
void vector_init(vector *v);

#endif // VECTOR_H
