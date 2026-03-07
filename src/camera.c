#include "camera.h"

Camera camInit() {
    Camera o;
    o.sensitivity = 0.1f;
    glm_quat_identity(o.orientation);
    glm_vec3_zero(o.position);
    return o;
}

Camera rotate(Camera cam, float angle, vec3 axis) {
    Camera o = cam;
    versor tmp;
    glm_vec3_normalize(axis);
    glm_quatv(tmp, angle, axis);
    glm_quat_mul(o.orientation, tmp, o.orientation);
    glm_quat_normalize(o.orientation);
    return o;
}

Camera translateWithOrientation(Camera cam, vec3 v) {
    Camera o = cam;
    vec3 rotated;
    glm_quat_rotatev(o.orientation, v, rotated);
    glm_vec3_add(o.position, rotated, o.position);
    return o;
}

Camera translate(Camera cam, vec3 v) {
    Camera o = cam;
    glm_vec3_add(o.position, v, o.position);
    return o;
}

void cameraToViewMatrix(Camera cam, mat4 dest) {
    glm_quat_look(cam.position, cam.orientation, dest);
}

Camera processMouse(Camera cam, float xoff, float yoff) {
    Camera o = cam;

    xoff *= cam.sensitivity;
    yoff *= cam.sensitivity;

    vec3 up = {0.0f, -1.0f, 0.0f};
    o = rotate(o, glm_rad(xoff), up);

    vec3 forward = {0.0f, 0.0f, -1.0f};
    glm_quat_rotatev(o.orientation, forward, forward);

    vec3 right = {1.0f, 0.0f, 0.0f};
    glm_quat_rotatev(o.orientation, right, right);

    float currentPitch = asinf(glm_clamp(forward[1], -1.0f, 1.0f));
    float newPitch = currentPitch + glm_rad(-yoff);
    newPitch = glm_clamp(newPitch, -M_PI / 2.0f + 0.01f, M_PI / 2.0f - 0.01f);
    float delta = newPitch - currentPitch;

    o = rotate(o, delta, right);

    return o;
}
