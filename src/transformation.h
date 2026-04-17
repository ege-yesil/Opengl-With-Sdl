#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include "../cglm/cglm.h"

typedef struct {
    vec3 position;
    versor orientation;
    vec3 scale;
    mat4 model;
    bool updated;
} Transformation;

void translate(Transformation *this, vec3 v);
void translateWithOrientation(Transformation *this, vec3 v);
void scale(Transformation *this, vec3 v);
void rotate(Transformation *this, float angle, vec3 axis);

#endif
