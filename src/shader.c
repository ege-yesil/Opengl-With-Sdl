#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#include "shader.h"
#include "util/string.h"

unsigned int loadTexture(const char *path) {
    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (!data) {
        printf("\nfailed to load texture");
        stbi_image_free(data);
        return 0;
    }

    unsigned int o;
    glGenTextures(1, &o);
    
    GLenum format;
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

unsigned int loadShader(const char *path, GLenum shaderType) {
    const char *src = readFile(path);
    if (src == NULL) {
        return -1;
    }
    unsigned int out = glCreateShader(shaderType);
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

unsigned int createShaderProgram(unsigned int vertex, unsigned int fragment) {
    unsigned int out = glCreateProgram();
    glAttachShader(out, vertex);
    glAttachShader(out, fragment);

    glLinkProgram(out);

    return out;
}
