#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>

unsigned int loadTexture(const char *path);
unsigned int loadShader(const char *path, GLenum shaderType);
unsigned int createShaderProgram(unsigned int vertex, unsigned int fragment); // deletes vertex and fragment shaders

#endif
