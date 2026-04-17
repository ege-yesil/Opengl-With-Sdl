#ifndef SHADER_H
#define SHADER_H
#include <stdint.h>
#include <glad/glad.h>
#include "util/hashMap.h"

#define CHECK_UNIFORM(loc, name) \
    if ((loc) == -1) { \
        error++; \
        printf("shader uniform %s missing from shader program %u\n", name, this.program); \
    }

#define NR_TEXTURE_MAPS 3

typedef struct {
    int32_t model;
    int32_t view;
    int32_t projection;
} VertexUniforms;

// to add more fragment uniforms use a union
typedef struct {
    int32_t diffuseMaps[NR_TEXTURE_MAPS];
    int32_t specularMaps[NR_TEXTURE_MAPS];
    int32_t ambient;
    int32_t diffuse;
    int32_t specular;
    int32_t shininess;
} PhongMaterialUniforms;

typedef struct {
    uint32_t program;
    VertexUniforms vertexUniforms;
    PhongMaterialUniforms materialUniforms;
} Shader;

enum LightingSystem {
    NONE,
    PHONG
};

uint32_t loadTexture(const char *path);
uint32_t loadShader(const char *path, GLenum shaderType);
size_t checkUniforms(Shader this);
void setupFragmentUniforms(Shader *this, enum LightingSystem system);
void setupVertexUniforms(Shader *this, size_t width, size_t height);
Shader createShader(uint32_t vertex, uint32_t fragment);

#endif
