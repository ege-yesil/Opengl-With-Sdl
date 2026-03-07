#ifndef CAMERA_H
#define CAMERA_H

#include "../cglm/cglm.h"

typedef struct {
    vec3 position;    
    versor orientation;
    float sensitivity;
} Camera;

Camera camInit();
Camera rotate(Camera cam, float angle, vec3 axis);
Camera translateWithOrientation(Camera cam, vec3 v);
Camera translate(Camera cam, vec3 v);
void cameraToViewMatrix(Camera cam, mat4 dest);

Camera processMouse(Camera cam, float xoff, float yoff);

#endif
