#include "transformation.h"

void translate(Transformation *this, vec3 v) {
    glm_vec3_add(this->position, v, this->position);
    this->updated = true;
}

void translateWithOrientation(Transformation *this, vec3 v) {
    vec3 rotated;
    glm_quat_rotatev(this->orientation, v, rotated);
    glm_vec3_add(this->position, rotated, this->position);
    this->updated = true;
}

void scale(Transformation *this, vec3 v) {
    glm_vec3_copy(v, this->scale);
    this->updated = true;
}

void rotate(Transformation *this, float angle, vec3 axis) {
    versor tmp;
    glm_vec3_normalize(axis);
    glm_quatv(tmp, angle, axis);
    glm_quat_mul(this->orientation, tmp, this->orientation);
    glm_quat_normalize(this->orientation); 
    this->updated = true;
}
