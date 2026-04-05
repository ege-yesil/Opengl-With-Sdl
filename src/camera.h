#ifndef CAMERA_H
#define CAMERA_H
#include "../cglm/cglm.h"

#include "transformation.h"

typedef struct {
    Transformation transformation;
    float sensitivity;
} Camera;

Camera camInit();
void cameraToViewMatrix(Camera cam, mat4 dest);

Camera processMouse(Camera cam, float xoff, float yoff);

#endif
