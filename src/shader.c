#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#include <cglm.h>
#include "shader.h"
#include "util/string.h"

uint32_t loadTexture(const char *path) {
    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (!data) {
        printf("\nfailed to load texture");
        stbi_image_free(data);
        return 0;
    }

    uint32_t o;
    glGenTextures(1, &o);
    
    GLenum format = GL_RGB;
    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, o);
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_image_free(data);

    return o;
}

uint32_t loadShader(const char *path, GLenum shaderType) {
    const char *src = readFile(path);
    if (src == NULL) {
        return -1;
    }
    uint32_t out = glCreateShader(shaderType);
    glShaderSource(out, 1, &src, NULL);    
    glCompileShader(out);
    
    int success;
    char infoLog[512];
    glGetShaderiv(out, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(out, 512, NULL, infoLog);
        printf("Shader compile error:\n%s\n", infoLog);
    }
    
    free((char*)src);
    return out;
}

size_t checkUniforms(Shader this) {
    size_t error = 0;

    CHECK_UNIFORM(this.vertexUniforms.model, "model");
    CHECK_UNIFORM(this.vertexUniforms.view, "view");
    CHECK_UNIFORM(this.vertexUniforms.projection, "projection");

    CHECK_UNIFORM(this.materialUniforms.ambient, "material.ambientVec");
    CHECK_UNIFORM(this.materialUniforms.diffuse, "material.diffuseVec");
    CHECK_UNIFORM(this.materialUniforms.specular, "material.specularVec");
    
    for (int i = 0; i < NR_TEXTURE_MAPS; i++) {
        char uniform[128];
        sprintf(uniform, "material.diffuse[%d]", i);
        CHECK_UNIFORM(this.materialUniforms.diffuseMaps[i], uniform);
        sprintf(uniform, "material.specular[%d]", i);
        CHECK_UNIFORM(this.materialUniforms.specularMaps[i], uniform); 
    } 
    return error;
}

void setupFragmentUniforms(Shader *this, enum LightingSystem system) {
    glUseProgram(this->program);
    if (system == PHONG) {
        for (int i = 0; i < NR_TEXTURE_MAPS; i++) {
            char uniform[128];
            sprintf(uniform, "material.diffuse[%d]", i);
            this->materialUniforms.diffuseMaps[i] = glGetUniformLocation(this->program, uniform);
            sprintf(uniform, "material.specular[%d]", i);
            this->materialUniforms.specularMaps[i] = glGetUniformLocation(this->program, uniform);
        }

        this->materialUniforms.ambient = glGetUniformLocation(this->program, "material.ambientVec");
        this->materialUniforms.diffuse = glGetUniformLocation(this->program, "material.diffuseVec");
        this->materialUniforms.specular = glGetUniformLocation(this->program, "material.specularVec");
        this->materialUniforms.shininess = glGetUniformLocation(this->program, "material.shininess");
    }
}

void setupVertexUniforms(Shader *this, size_t width, size_t height) {
    glUseProgram(this->program);
    this->vertexUniforms.model = glGetUniformLocation(this->program, "model"); 
    this->vertexUniforms.view = glGetUniformLocation(this->program, "view");
    this->vertexUniforms.projection = glGetUniformLocation(this->program, "projection"); 

    //setup projection
    mat4 projection = GLM_MAT4_IDENTITY_INIT;
    glm_perspective(M_PI / 4.0F, (float)width / (float)height, 0.1f, 100.0f, projection);
    glUniformMatrix4fv(this->vertexUniforms.projection, 1, GL_FALSE, projection[0]);
}

Shader createShader(uint32_t vertex, uint32_t fragment) {
    Shader out = { 0 };
    out.program  = glCreateProgram();
    glAttachShader(out.program, vertex);
    glAttachShader(out.program, fragment);
    glLinkProgram(out.program);

    return out;
}
