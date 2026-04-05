/* How to use the mesh system
 * Object object = loadObject("assets/mesh.obj");
 * initObject(&mesh, shaderProgram, hasMaterial, winWidth, winHeight);
 * 
 * inside the draw loop do
 * updateObject(&object, camera, shaderProgram);
 * drawObject(&Object, shaderProgram);
 */

#ifndef OBJECT_H
#define OBJECT_H
#include <stdint.h>
#include "../cglm/cglm.h" 

#include "shader.h"
#include "util/vector.h"
#include "util/string.h"
#include "transformation.h"
#include "camera.h"

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
    uint32_t id;
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
    Vector indices;  // Vector of type uint32_t 
    Vector subMeshes; // Vector of type Submesh 
   
    uint32_t vao, vbo, ebo;
} Mesh;

typedef struct {
    Mesh mesh;
    Transformation transformation;
   
    Vector materials; // Vector of type PhongMaterial
    bool hasMaterial; 
    // members of shader material struct 
    int32_t materialUniforms[4 + NR_TEXTURE_MAPS * 2]; // uniform to shader material 
    /* LAYOUT
     * diffuse maps * NR_TEXTURE_MAPS: sampler2D 
     * specular maps * NR_TEXTURE_MAPS: sampler2D
     * ambient: vec3
     * diffuse: vec3
     * specular: vec3
     * shininess: float
     */
    int32_t vertexUniforms[3]; // vertex shader uniforms
    /* LAYOUT
     * model
     * view
     * projection
     */
} Object;

// TODO: add emissive material loading and emission maps general
// TODO: add index of  refraction material loading
// TODO: add transparancy
// TODO: add illum model
Vector loadMtlMesh(const char *path);  // returns a vector of PhongMaterial
Object loadObject(const char *path);
void drawObject(Object *this, uint32_t shader);
void deletePhongMaterial(PhongMaterial *material);
void updateObject(Object *this, Camera cam, uint32_t shader);
void initMesh(Mesh *this); // bind opengl buffers
void initObject(Object *this, uint32_t shader, bool hasMaterial, float winWidth, float winHeight); // setup uniform maps setup projection matrix 

#endif
