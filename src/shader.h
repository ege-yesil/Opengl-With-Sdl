#ifndef SHADER_H
#define SHADER_H
#include <stdint.h>
#include <glad/glad.h>

uint32_t loadTexture(const char *path);
uint32_t loadShader(const char *path, GLenum shaderType);
uint32_t createShaderProgram(uint32_t vertex, uint32_t fragment); // deletes vertex and fragment shaders

#endif
