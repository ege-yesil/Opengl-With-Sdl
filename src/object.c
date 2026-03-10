#include <glad/glad.h>
#include <stddef.h>
#include <string.h>
#include "../cglm/cglm.h"

#include "object.h"
#include "util/hashMap.h"

uint32_t loadMtlTexture(FILE *file, const char *path) {
    char tex[128];
    fscanf(file, "%s\n", &tex);
    String parent = getParentDir(path);
    String full = mergeDir(parent.str, tex);
    uint32_t o = loadTexture(full.str);
    free(parent.str);
    free(full.str);
    return o; 
}

Vector loadMtlMesh(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) return makeVec(0, 0);

    Vector o = makeVec(8, sizeof(PhongMaterial));
    size_t currentIndex = 0;
    size_t dMap = 0, sMap = 0;
    PhongMaterial push = { 0 };
    while (1) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        
        if (strcmp(lineHeader, "newmtl") == 0) {
            if (push.name.size != 0) pushVec(&o, &push, 1);
            char name[128];
            fscanf(file, "%s\n", &name);
            push.name = makeStr(name);
        } else if (strcmp(lineHeader, "Ns") == 0) {
            fscanf(file, "%f\n", &push.shininess);
        } else if (strcmp(lineHeader, "Ka") == 0) {
            fscanf(file, "%f %f %f\n", &push.ambient[0], &push.ambient[1], &push.ambient[2]);
        } else if (strcmp(lineHeader, "Kd") == 0) {
            fscanf(file, "%f %f %f\n", &push.diffuse[0], &push.diffuse[1], &push.diffuse[2]);
        } else if (strcmp(lineHeader, "Ks") == 0) {
            fscanf(file, "%f %f %f\n", &push.specular[0], &push.specular[1], &push.specular[2]);
        } else if (strcmp(lineHeader, "map_Kd") == 0) {
            push.diffuseMaps[dMap].id = loadMtlTexture(file, path);
            push.diffuseMaps[dMap].type = TEX_DIFFUSE;
            dMap++;
        } else if (strcmp(lineHeader, "map_Ks") == 0) {
            push.specularMaps[sMap].id = loadMtlTexture(file, path);
            push.specularMaps[sMap].type = TEX_DIFFUSE;
            sMap++;
        }
    }
    // one final push
    pushVec(&o, &push, 1);
     
    fclose(file);
    return o; 
}

Mesh loadObjMesh(const char *path) {
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
    o.indices = makeVec(128, sizeof(uint32_t));
    o.subMeshes = makeVec(128, sizeof(SubMesh));
    HashMap vertMap = makeHashMap(sizeof(VertexKey), sizeof(uint32_t));
    vertMap.hash = vertexKeyHash;
    vertMap.equals = equalsVertexKeyHashMap;
    SubMesh currentSubMesh = { 0 };
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
                currentSubMesh.indexCount++;
            }
        } else if (strcmp(lineHeader, "mtllib") == 0) {
            char mtlFile[128];
            fscanf(file, "%s\n", &mtlFile); 
            String parent = getParentDir(path); 
            String full = mergeDir(parent.str, mtlFile); 
            o.materials = loadMtlMesh(full.str);
            free(full.str);
            free(parent.str);
        } else if (strcmp(lineHeader, "usemtl") == 0) {
            char mtl[128];
            fscanf(file, "%s\n", &mtl);
            for (size_t i = 0; i < o.materials.size; i++) {
                PhongMaterial material = *(PhongMaterial*)getVec(&o.materials, i);
                if (strcmp(material.name.str, mtl) == 0) {
                    if (currentSubMesh.indexCount !=  0)
                        pushVec(&o.subMeshes, &currentSubMesh, 1); 
                    currentSubMesh.materialIndex = i;
                    currentSubMesh.indexOffset = o.indices.size;
                }
            }
        }
    }
    fclose(file);

    free(tempPos.data);
    free(tempUv.data);
    free(tempNormal.data);
    freeHashMap(&vertMap);
    
    return o;
}

void bindTexture(Texture tex, size_t glTextureIndex, int32_t location) {
    glActiveTexture(GL_TEXTURE0 + glTextureIndex);
    if (tex.id != 0) 
        glBindTexture(GL_TEXTURE_2D, tex.id);
     else
         glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(location, glTextureIndex);
}

void drawMesh(Mesh *this, uint32_t shader) {
    glUseProgram(shader);
    glBindVertexArray(this->vao);
    
    if (this->hasMaterial) {
        for (size_t i = 0; i < this->subMeshes.size; i++) {
            SubMesh *sub = (SubMesh*)getVec(&this->subMeshes, i);
            PhongMaterial *mat = (PhongMaterial*)getVec(&this->materials, sub->materialIndex);
            for (int32_t j = 0; j < NR_TEXTURE_MAPS; j++) {
                bindTexture(mat->diffuseMaps[j], j, this->matUniform[j]);
                bindTexture(mat->specularMaps[j], NR_TEXTURE_MAPS + j, this->matUniform[NR_TEXTURE_MAPS + j]);
            }
            glUniform3fv(this->matUniform[NR_TEXTURE_MAPS * 2], 1, &mat->ambient[0]);
            glUniform3fv(this->matUniform[NR_TEXTURE_MAPS * 2 + 1], 1, &mat->diffuse[0]);
            glUniform3fv(this->matUniform[NR_TEXTURE_MAPS * 2 + 2], 1, &mat->specular[0]); 
            glUniform1f(this->matUniform[NR_TEXTURE_MAPS * 2 + 3], mat->shininess); 
            glDrawElements(GL_TRIANGLES, sub->indexCount, GL_UNSIGNED_INT, (void*)(sub->indexOffset  * sizeof(uint32_t)));
        }
    } else {
        glDrawElements(GL_TRIANGLES, this->indices.size, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void deletePhongMaterial(PhongMaterial *material) {
    free(material->name.str);
}

void initMesh(Mesh *this, uint32_t shader, bool hasMaterial) {
    this->hasMaterial = hasMaterial;
    if (hasMaterial) {
        glUseProgram(shader);
        for (int i = 0; i < NR_TEXTURE_MAPS; i++) {
            char uniform[128];
            sprintf(uniform, "material.diffuse[%d]", i);
            this->matUniform[i] = glGetUniformLocation(shader, uniform);
            sprintf(uniform, "material.specular[%d]", i);
            this->matUniform[NR_TEXTURE_MAPS + i] = glGetUniformLocation(shader, uniform);
        }
        
        this->matUniform[NR_TEXTURE_MAPS * 2] = glGetUniformLocation(shader, "material.ambientVec");
        this->matUniform[NR_TEXTURE_MAPS * 2 + 1] = glGetUniformLocation(shader, "material.diffuseVec");
        this->matUniform[NR_TEXTURE_MAPS * 2 + 2] = glGetUniformLocation(shader, "material.specularVec");
        this->matUniform[NR_TEXTURE_MAPS * 2 + 3] = glGetUniformLocation(shader, "material.shininess");
        for (size_t i = 0; i < NR_TEXTURE_MAPS * 2 + 4; i++) {
            if (this->matUniform[i] == -1) printf("Could not recieve uniform for %d index\n", i);
        }
    }

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
