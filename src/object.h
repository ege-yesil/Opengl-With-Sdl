/* How to use the mesh system
 * Mesh mesh = loadObjMesh("assets/mesh.obj");
 * initMesh(&mesh);
 * 
 * inside the draw loop do
 * drawMesh(&mesh, shaderProgram);
 */

#ifndef OBJECT_H
#define OBJECT_H
#include "../cglm/cglm.h" 

#include "shader.h"
#include "util/vector.h"

typedef struct {
    vec3 pos;
    vec3 normal;
    vec2 uv;
} Vertex;

enum TextureType {
    TEX_DIFFUSE, 
    TEX_SPECULAR,
};

typedef struct {
    unsigned int id;
    enum TextureType type;
} Texture;

typedef struct {
    Vector vertices; // Vector of type Vertex
    Vector indices;  // Vector of type unsigned int
    Vector textures; // Vector of type Texture

    unsigned int vao, vbo, ebo;
} Mesh;

// TODO: implement hash map to make the obj loading algorithm better
Mesh loadObjMesh(char *path);
void drawMesh(Mesh *this, unsigned int shader);
void initMesh(Mesh *this);

#endif
