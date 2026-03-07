#include <stdlib.h>
#include <string.h>

#include "vector.h"

uint8_t makeVec(Vector *v, size_t size, size_t stride) {
    v->data = malloc(size * stride);
    if (!v->data) return 1;
    v->size = 0;
    v->capacity = size;
    v->stride = stride;
    return 0;
}

uint8_t reserveVec(Vector *v, size_t count) {
    if (count <= v->capacity)
        return 0;
    size_t newSize = v->capacity;
    if (newSize == 0) newSize = 16;
    while (newSize < count)
        newSize *= 2;
    
    void *new = realloc(v->data, newSize * v->stride);
    if (!new) return 1;
    v->data = new;
    v->capacity = newSize; 
    return 0;
}

uint8_t pushVec(Vector *v, void *data, size_t count) {
    size_t needed = v->size + count;
    if (reserveVec(v, needed) != 0) return 1;

    memcpy((char*)v->data + v->size * v->stride, data, count * v->stride);
    
    v->size += count;
    return 0;
}

uint8_t popVec(Vector *v, size_t count) {
    size_t allocCount = v->size- count;
    if (allocCount < 0)
        return 1;
    v->size -= count;
    return 0;
}

void writeVec(Vector *v, void *elem, size_t index) {
    if (index >= v->capacity) return;
    memcpy((char*)v->data + index * v->stride, elem, v->stride); 
}

void *getVec(Vector *v, size_t index) {
    if (index >= v->size) return NULL;
    return (char*)v->data + index * v->stride; 
}
