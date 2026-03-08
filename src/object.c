#include <glad/glad.h>
#include <stddef.h>
#include <string.h>
#include "../cglm/cglm.h"

#include "object.h"
#include "util/string.h"
#include "util/hashMap.h"

Mesh loadObjMesh(char *path) {
    Mesh o;
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("object file cannot be accessed or not present in the directory %s\n", path);
        return o;
    }

    Vector tempPos = makeVec(128, sizeof(vec3));
    Vector tempUv = makeVec(128, sizeof(vec2));
    Vector tempNormal = makeVec(128, sizeof(vec3));
    
    o.vertices = makeVec(128, sizeof(Vertex));
    o.textures = makeVec(16, sizeof(Texture));
    o.indices = makeVec(128, sizeof(unsigned int));
    HashMap vertMap = makeHashMap(sizeof(VertexKey), sizeof(unsigned int));
    vertMap.hash = vertexKeyHash;
    vertMap.equals = equalsVertexKeyHashMap;
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
                VertexKey key = { vertexIndex[i] - 1, normalIndex[i] - 1, uvIndex[i] - 1};
                size_t existingIndex = getHashMap(&vertMap, &key);
                if (existingIndex != SIZE_MAX) {
                    pushVec(&o.indices, vertMap.entries[existingIndex].val, 1);
                } else {
                    Vertex vert;
                    vec3 *v = getVec(&tempPos, key.v);
                    vec3 *vn = getVec(&tempNormal, key.vn);
                    vec2 *vt = getVec(&tempUv, key.vt);
                    glm_vec3_copy(*v, vert.pos);
                    glm_vec3_copy(*vn, vert.normal);
                    glm_vec2_copy(*vt, vert.uv);
                    vert.uv[1] = 1.0f - vert.uv[1];
                    vert.pos[1] = 1.0f - vert.pos[1];
                    
                    unsigned int newIndex = o.vertices.size;
                    pushVec(&o.vertices, &vert, 1);
                    addHashMap(&vertMap, &key, &newIndex);
                    pushVec(&o.indices, &newIndex, 1);
                }
            }
        }
    }
    free(tempPos.data);
    free(tempUv.data);
    free(tempNormal.data);

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
