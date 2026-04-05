#include "camera.h"

Camera camInit() {
    Camera o;
    o.sensitivity = 0.1f;
    glm_quat_identity(o.transformation.orientation);
    glm_vec3_zero(o.transformation.position);
    return o;
}
/*
Camera rotate(Camera cam, float angle, vec3 axis) {
    Camera o = cam;
    versor tmp;
    glm_vec3_normalize(axis);
    glm_quatv(tmp, angle, axis);
    glm_quat_mul(o.orientation, tmp, o.orientation);
    glm_quat_normalize(o.orientation);
    return o;
}

Camera translateWithOrientationCamera(Camera cam, vec3 v) {
    Camera o = cam;
    vec3 rotated;
    glm_quat_rotatev(o.orientation, v, rotated);
    glm_vec3_add(o.position, rotated, o.position);
    return o;
}

Camera translateCamera(Camera cam, vec3 v) {
    Camera o = cam;
    glm_vec3_add(o.position, v, o.position);
    return o;
}
*/
void cameraToViewMatrix(Camera cam, mat4 dest) {
    glm_quat_look(cam.transformation.position, cam.transformation.orientation, dest);
}

Camera processMouse(Camera cam, float xoff, float yoff) {
    Camera o = cam;

    xoff *= cam.sensitivity;
    yoff *= cam.sensitivity;

    vec3 up = {0.0f, -1.0f, 0.0f};
    rotate(&o.transformation, glm_rad(xoff), up);

    vec3 forward = {0.0f, 0.0f, -1.0f};
    glm_quat_rotatev(o.transformation.orientation, forward, forward);

    vec3 right = {1.0f, 0.0f, 0.0f};
    glm_quat_rotatev(o.transformation.orientation, right, right);

    float currentPitch = asinf(glm_clamp(forward[1], -1.0f, 1.0f));
    float newPitch = currentPitch + glm_rad(-yoff);
    newPitch = glm_clamp(newPitch, -M_PI / 2.0f + 0.01f, M_PI / 2.0f - 0.01f);
    float delta = newPitch - currentPitch;

    rotate(&o.transformation, delta, right);

    return o;
}
