#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "../cglm/cglm.h"

#include "shader.h"
#include "camera.h"
#include "util/vector.h"
#include "object.h"
#include "util/hashMap.h"

float cube[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

SDL_Window *setupSDL(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init fail");
        return NULL;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_Window *win = SDL_CreateWindow("Opengl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!win) {
        printf("SDL_CreateWindow fail");
        return NULL;
    }
    
    SDL_GLContext glctx = SDL_GL_CreateContext(win);
    if (!glctx) {
        printf("SDL_GL_CreateContext fail");
        return NULL;
    }
    SDL_GL_MakeCurrent(win, glctx);
    return win;
}

struct PointLight {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
}; 

void test() {
    HashMap map = makeHashMap(sizeof(VertexKey), sizeof(int));
    map.hash = intHash;
    map.equals = equalsVertexKeyHashMap;

    VertexKey key = { 0, 2, 3 };
    int val = 2;
    addHashMap(&map, &key, &val);

    size_t i = getHashMap(&map, &key);
    printf("str: %d\n", *(int*)map.entries[i].val);


    exit(69);
}

int main() {
    test();
    float winWidth = 1080, winHeight = 800;
    SDL_Window *win = setupSDL(winWidth, winHeight);   
    
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Failed to load GL\n");
        return 1;
    }
    printf("GL version: %s\n", glGetString(GL_VERSION));
    
    glEnable(GL_DEPTH_TEST);     
    
    unsigned int vert = loadShader("src/shaders/vertex.glsl", GL_VERTEX_SHADER);
    if (vert == 0) printf("\ncouldnt load vertex shader");
    unsigned int objFrag = loadShader("src/shaders/objectFragment.glsl", GL_FRAGMENT_SHADER);
    if (objFrag == 0) printf("\ncouldnt load fragment shader");  
    unsigned int lightFrag = loadShader("src/shaders/lightFragment.glsl", GL_FRAGMENT_SHADER); 
    if (lightFrag == 0) printf("\ncouldnt load fragment shader");  
    unsigned int objProgram = createShaderProgram(vert, objFrag);
    unsigned int lightProgram = createShaderProgram(vert, lightFrag);
    glDeleteShader(vert);
    glDeleteShader(objFrag);
    glDeleteShader(lightFrag);
    
    Mesh backpack = loadObjMesh("assets/backpack.obj");
    Mesh light = loadObjMesh("assets/cube.obj");
    initMesh(&backpack);
    initMesh(&light);
   
    unsigned int diffuseMap = loadTexture("assets/box.png");
    unsigned int specularMap = loadTexture("assets/boxSpecular.png");
    Texture diffMap = { diffuseMap, TEX_DIFFUSE };
    Texture specMap = { specularMap, TEX_SPECULAR };
    pushVec(&backpack.textures, (void*)&diffMap, 1);
    pushVec(&backpack.textures, (void*)&specMap, 1);

    glUseProgram(objProgram);
    glUniform3f(glGetUniformLocation(objProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(objProgram, "material.shininess"), 32.0f);
    
    glUniform3f(glGetUniformLocation(objProgram, "light.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(objProgram, "light.diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(objProgram, "light.specular"), 1.0f, 1.0f, 1.0f);
    
    glUniform1f(glGetUniformLocation(objProgram, "light.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(objProgram, "light.linear"), 0.009f);
    glUniform1f(glGetUniformLocation(objProgram, "light.quadratic"), 0.032f);

        glUseProgram(objProgram);
    glUniform1i(glGetUniformLocation(objProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(objProgram, "material.specular"), 1);

    // camera and transformations 
    Camera cam = camInit();
    cam.sensitivity = 0.2;
    cam = translate(cam, (vec3){ 0.0f, -1.0f, -3.0f });
    cam = rotate(cam, M_PI, (vec3){ 1.0f, 0.0f, 0.0f });
    mat4 model = GLM_MAT4_IDENTITY_INIT, view = GLM_MAT4_IDENTITY_INIT, projection = GLM_MAT4_IDENTITY_INIT;
    cameraToViewMatrix(cam, view);
    glm_perspective(M_PI / 4.0f, winWidth / winHeight, 0.1f, 100.0f, projection);

    vec3 lightPos = { 4.0f, -3.0f, -1.0f};
    glUniformMatrix4fv(glGetUniformLocation(objProgram, "projection"), 1, GL_FALSE, projection[0]);
    glUniform3fv(glGetUniformLocation(objProgram, "lightPos"), 1, lightPos);
    
    
    glUseProgram(lightProgram); 
    glUniformMatrix4fv(glGetUniformLocation(lightProgram, "projection"), 1, GL_FALSE, projection[0]);
    mat4 modelLight = GLM_MAT4_IDENTITY_INIT;
    glm_translate_make(modelLight, lightPos);
    glUniformMatrix4fv(glGetUniformLocation(lightProgram, "model"), 1, GL_FALSE, modelLight[0]);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   
    // final touches
    srand(time(0));
    vec3 positions[10];
    for (int i = 0; i < 10; i++) {
        positions[i][0] = (rand() % 10) - 5;
        positions[i][1] = (rand() % 6) - 3;
        positions[i][2] = (rand() % 6) - 3;
    }
    
    struct PointLight lights[3];
    lights[0] = (struct PointLight){
        .pos = {5.0f, 2.0f, 0.0f},
        .ambient = { 0.2f, 0.2f, 0.2f },
        .diffuse = { 1.0f, 1.0f, 1.0f },
        .specular = { 1.0f, 1.0f, 1.0f },
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f
    };
    lights[1] = (struct PointLight){
        .pos = { -5.0f, 0.0f, -4.0f},
        .ambient = { 0.3f, 0.3f, 0.3f },
        .diffuse = { 0.5f, 0.5f, 0.5f },
        .specular = { 1.0f, 1.0f, 1.0f },
        .constant = 1.0f,
        .linear = 0.009f,
        .quadratic = 0.032f
    };
   lights[2] = (struct PointLight){
        .pos = { 0.0f, -4.0f, 4.0f},
        .ambient = { 0.2f, 0.2f, 0.2f },
        .diffuse = { 0.4f, 0.4f, 0.4f },
        .specular = { 0.5f, 0.5f, 0.5f },
        .constant = 1.0f,
        .linear = 0.1f,
        .quadratic = 0.0032f
    };
   
    glUseProgram(objProgram); 
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[0].position"), 1, lights[0].pos);
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[0].ambient"), 1, lights[0].ambient);
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[0].diffuse"), 1, lights[0].diffuse);
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[0].specular"), 1, lights[0].specular);
    glUniform1f(glGetUniformLocation(objProgram, "pointLights[0].constant"), lights[0].constant);
    glUniform1f(glGetUniformLocation(objProgram, "pointLights[0].linear"), lights[0].linear);
    glUniform1f(glGetUniformLocation(objProgram, "pointLights[0].quadratic"), lights[0].quadratic);

    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[1].position"), 1, lights[1].pos);
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[1].ambient"), 1, lights[1].ambient);
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[1].diffuse"), 1, lights[1].diffuse);
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[1].specular"), 1, lights[1].specular);
    glUniform1f(glGetUniformLocation(objProgram, "pointLights[1].constant"), lights[1].constant);
    glUniform1f(glGetUniformLocation(objProgram, "pointLights[1].linear"), lights[1].linear);
    glUniform1f(glGetUniformLocation(objProgram, "pointLights[1].quadratic"), lights[1].quadratic);
    
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[2].position"), 1, lights[2].pos);
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[2].ambient"), 1, lights[2].ambient);
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[2].diffuse"), 1, lights[2].diffuse);
    glUniform3fv(glGetUniformLocation(objProgram, "pointLights[2].specular"), 1, lights[2].specular);
    glUniform1f(glGetUniformLocation(objProgram, "pointLights[2].constant"), lights[2].constant);
    glUniform1f(glGetUniformLocation(objProgram, "pointLights[2].linear"), lights[2].linear);
    glUniform1f(glGetUniformLocation(objProgram, "pointLights[2].quadratic"), lights[2].quadratic);
   
    glUniform3f(glGetUniformLocation(objProgram, "dirLight.direction"), 0.2f, -0.4f, 0.2f);
    glUniform3f(glGetUniformLocation(objProgram, "dirLight.ambient"), 0.1f, 0.0f, 0.2f);
    glUniform3f(glGetUniformLocation(objProgram, "dirLight.diffuse"), 0.2f, 0.5f, 1.0f);
    glUniform3f(glGetUniformLocation(objProgram, "dirLight.specular"), 0.0f, 1.0f, 1.0f);
    
    unsigned long now = SDL_GetPerformanceCounter();
    unsigned long last = 0;
    double deltaTime = 0;
    int running = 1;
    const float speed = 10.0f;

    // for camera
//    SDL_SetRelativeMouseMode(SDL_TRUE);
    while (running) {
        now = SDL_GetPerformanceCounter();
        deltaTime = (now - last) / (double)SDL_GetPerformanceFrequency();
        last = now;
        printf("\rDelta time: %f, fps: %d", deltaTime, (int)(1000 / deltaTime));
        printf(" Coordinates: x=%.2f y=%.2f z=%.2f", cam.position[0], cam.position[1], cam.position[2]);
        const unsigned char *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_W])
            cam = translateWithOrientation(cam, (vec3){ 0.0f, 0.0f, -speed * deltaTime });
        if (keystate[SDL_SCANCODE_S])
            cam = translateWithOrientation(cam, (vec3){ 0.0f, 0.0f, speed * deltaTime });
        if (keystate[SDL_SCANCODE_A])
            cam = translateWithOrientation(cam, (vec3){ -speed * deltaTime, 0.0f, 0.0f });
        if (keystate[SDL_SCANCODE_D])
            cam = translateWithOrientation(cam, (vec3){ speed * deltaTime, 0.0f, 0.0f });
        if (keystate[SDL_SCANCODE_SPACE])
            cam = translateWithOrientation(cam, (vec3){ 0.0f, speed * deltaTime, 0.0f });
        if (keystate[SDL_SCANCODE_LSHIFT])
            cam = translateWithOrientation(cam, (vec3){ 0.0f, -speed * deltaTime, 0.0f });
       
        if (keystate[SDL_SCANCODE_RIGHT])
            cam = rotate(cam, M_PI / 4 * deltaTime, (vec3){ 0.0f, -1.0f, 0.0f });
        if (keystate[SDL_SCANCODE_LEFT])
            cam = rotate(cam, M_PI / 4 * deltaTime, (vec3){ 0.0f, 1.0f, 0.0f });
        if (keystate[SDL_SCANCODE_UP])
            cam = rotate(cam, M_PI / 4 * deltaTime, (vec3){ 1.0f, 0.0f, 0.0f });
        if (keystate[SDL_SCANCODE_DOWN])
            cam = rotate(cam, M_PI / 4 * deltaTime, (vec3){ -1.0f, 0.0f, 0.0f });
        
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;
            if (e.type == SDL_MOUSEMOTION) {
                float xoffset = e.motion.xrel;
                float yoffset = e.motion.yrel;
                cam = processMouse(cam, xoffset, yoffset);
            }
        }
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cameraToViewMatrix(cam, view); 
        glUseProgram(objProgram);
        glUniform3fv(glGetUniformLocation(objProgram, "viewPos"), 1, cam.position); 
        glUniform3fv(glGetUniformLocation(objProgram, "light.direction"), 1, lightPos);
        glUniformMatrix4fv(glGetUniformLocation(objProgram, "model"), 1, GL_FALSE, model[0]);
        glUniformMatrix4fv(glGetUniformLocation(objProgram, "view"), 1, GL_FALSE, view[0]);

        glUseProgram(lightProgram);
        for (int i = 0; i < 3; i++) {
            glm_translate_make(modelLight, lights[i].pos);
            glUniformMatrix4fv(glGetUniformLocation(lightProgram, "model"), 1, GL_FALSE, modelLight[0]);
            glUniformMatrix4fv(glGetUniformLocation(lightProgram, "view"), 1, GL_FALSE, view[0]);
            drawMesh(&light, lightProgram);
        }
        drawMesh(&backpack, objProgram); 
        GLenum err = glGetError();
        if (err != 0)
            printf("GL error: %d\n", err);
   
        SDL_GL_SwapWindow(win);
    }

    printf("\n");
    return 0;
}
