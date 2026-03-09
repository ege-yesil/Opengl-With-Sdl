/* How to use the mesh system
 * Mesh mesh = loadObjMesh("assets/mesh.obj");
 * initMesh(&mesh, shaderProgram);
 * 
 * inside the draw loop do
 * drawMesh(&mesh, shaderProgram);
 */

#ifndef OBJECT_H
#define OBJECT_H
#include "../cglm/cglm.h" 

#include "shader.h"
#include "util/vector.h"
#include "util/string.h"

#define NR_TEXTURE_MAPS 3

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
    Texture diffuseMaps[NR_TEXTURE_MAPS];
    Texture specularMaps[NR_TEXTURE_MAPS];
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    String name;
} PhongMaterial;

typedef struct {
    uint32_t materialIndex;
    size_t indexOffset;
    size_t indexCount;
} SubMesh;

typedef struct {
    Vector vertices; // Vector of type Vertex
    Vector indices;  // Vector of type unsigned int 
    Vector materials; // Vector of type PhongMaterial
    Vector subMeshes; // Vector of type Submesh 
    
    bool hasMaterial; 
    // members of shader material struct 
    int32_t matUniform[4 + NR_TEXTURE_MAPS * 2]; // uniform to shader material 
    /* LAYOUT
     * diffuse maps * NR_TEXTURE_MAPS: sampler2D 
     * specular maps * NR_TEXTURE_MAPS: sampler2D
     * ambient: vec3
     * diffuse: vec3
     * specular: vec3
     * shininess: float
     */

    uint32_t vao, vbo, ebo;
} Mesh;

// TODO: add emissive material loading and emission maps general
// TODO: add index of  refraction material loading
// TODO: add transparancy
// TODO: add illum model
Vector loadMtlMesh(const char *path);  // returns a vector of PhongMaterial
Mesh loadObjMesh(const char *path);
void drawMesh(Mesh *this, uint32_t shader);
void deletePhongMaterial(PhongMaterial *material);
void initMesh(Mesh *this, uint32_t shader, bool hasMaterial);

#endif
