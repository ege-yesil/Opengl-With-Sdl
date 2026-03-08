#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    void *data;
    size_t capacity;  // total allocated space in members
    size_t size; // members in use
    size_t stride;
} Vector;

Vector makeVec(size_t size, size_t stride);
uint8_t reserveVec(Vector *v, size_t count);
uint8_t pushVec(Vector *v, void *data, size_t count);
uint8_t popVec(Vector *v, size_t count);
void writeVec(Vector *v, void *elem, size_t index);
void *getVec(Vector *v, size_t index);

#endif
