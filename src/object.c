#include <glad/glad.h>
#include <stddef.h>
#include <string.h>
#include "../cglm/cglm.h"

#include "object.h"
#include "util/string.h"

Mesh loadObjMesh(char *path) {
    Mesh o;
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("object file cannot be accessed or not present in the directory %s\n", path);
        return o;
    }

    Vector tempPos;
    Vector tempUv;
    Vector tempNormal;
    makeVec(&tempPos, 128, sizeof(vec3));
    makeVec(&tempUv, 128, sizeof(vec2));
    makeVec(&tempNormal, 128, sizeof(vec3));
    
    Vector vertIndices;
    Vector uvIndices;
    Vector normalIndices;
    makeVec(&vertIndices, 128, sizeof(unsigned int));
    makeVec(&uvIndices, 128, sizeof(unsigned int));
    makeVec(&normalIndices, 128, sizeof(unsigned int));
    while (1) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;

        if (strcmp(lineHeader, "v") == 0) {
            vec3 v;
            fscanf(file, "%f %f %f\n", &v[0], &v[1], &v[2]);
            pushVec(&tempPos, (void*)&v, 1);
        } else if (strcmp(lineHeader, "vt") == 0) {
            vec2 vt;
            fscanf(file, "%f %f\n", &vt[0], &vt[1]); 
            pushVec(&tempUv, (void*)&vt, 1); 
        } else if (strcmp(lineHeader, "vn") == 0) {
            vec3 vn;
            fscanf(file, "%f %f %f\n", &vn[0], &vn[1], &vn[2]);            
            pushVec(&tempNormal, (void*)&vn, 1); 
        } else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], 
                    &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], 
                    &uvIndex[2], &normalIndex[2]); 
            if (matches != 9) {
                printf("OBJ file cannot be read by parser");
                return o;
            }
            for (int i = 0; i < 3; i++) {
                pushVec(&vertIndices, (void*)&vertexIndex[i], 1);
                pushVec(&uvIndices, (void*)&uvIndex[i], 1);
                pushVec(&normalIndices, (void*)&normalIndex[i], 1);
            }
        }
    }
    
    makeVec(&o.vertices, 128, sizeof(Vertex));
    // init for later use
    makeVec(&o.textures, 16, sizeof(Texture));
    makeVec(&o.indices, 32, sizeof(unsigned int));
    for (unsigned int i = 0; i < vertIndices.size; i++) {
        vec3 *vertex = (vec3*)getVec(&tempPos, *(unsigned int*)getVec(&vertIndices, i) - 1);
        vec3 *normal = (vec3*)getVec(&tempNormal, *(unsigned int*)getVec(&normalIndices, i) - 1);
        vec2 *uv = (vec2*)getVec(&tempUv, *(int*)getVec(&uvIndices, i) - 1);
        if (uv == NULL || normal == NULL || vertex == NULL) {
            printf("error while loading the normal uv or position data, sending the data as is");
            break;
        }

        Vertex pushVertex;
        glm_vec3_copy(*vertex, pushVertex.pos);
        glm_vec3_copy(*normal, pushVertex.normal);
        glm_vec2_copy(*uv, pushVertex.uv);
        
        pushVec(&o.vertices, (void*)&pushVertex, 1);
        
        pushVec(&o.indices, (void*)&i, 1);
    }

    free(tempPos.data);
    free(tempUv.data);
    free(tempNormal.data);
    free(vertIndices.data);
    free(uvIndices.data);
    free(normalIndices.data);

    return o;
}

void drawMesh(Mesh *this, unsigned int shader) {
    glUseProgram(shader);
    
    if (this->textures.size != 0) { 
        String diffuseMap, specMap;
        makeStr(&diffuseMap, "material.diffuse[");
        makeStr(&specMap, "material.specular[");
        
        size_t diffuseIndex = 0, specIndex = 0;
        for (int i = 0; i < this->textures.size; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            Texture texture = *(Texture*)getVec(&this->textures, i); 
            glBindTexture(GL_TEXTURE_2D, texture.id);
            
            if (texture.type == TEX_DIFFUSE) {
                appendStrN(&diffuseMap, diffuseIndex++);
                appendStrC(&diffuseMap, "]", 1);
                int loc = glGetUniformLocation(shader, diffuseMap.str);
                glUniform1i(loc, i); 
                popStr(&diffuseMap, 2); 
            }
            if (texture.type == TEX_SPECULAR) {
                appendStrN(&specMap, specIndex++);
                appendStrC(&specMap, "]", 1);
                int loc = glGetUniformLocation(shader, specMap.str);
                glUniform1i(loc, i); 
                popStr(&specMap, 2); 
            }
        }
    }
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, this->indices.size, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void initMesh(Mesh *this) {
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size * this->vertices.stride, this->vertices.data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size * this->indices.stride, this->indices.data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size * this->indices.stride, this->indices.data, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    
    glBindVertexArray(0);
}
