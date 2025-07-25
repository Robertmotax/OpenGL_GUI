// include necessary headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/util.h"
#include "core/Shader.h"
#include "core/RenderableObject.h"
#include "core/RenderableObjectStatic.h"
#include "core/LightSource.h"
#include "core/Tri.h"
#include "core/Camera.h"
#include "core/RayPicker.h"
#include "core/Texture.h"
#include "core/MouseClickHandler.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib> 
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// This library used for texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"


const char* vertexPath = "shaders/main.vert";
const char* fragmentPath = "shaders/main.frag";
const char* vertexPathUI = "shaders/UI.vert";
const char* fragmentPathUI = "shaders/UI.frag";
const char* vertexPathShadow = "shaders/shadow.vert";
const char* fragmentPathShadow = "shaders/shadow.frag";

//load new textures
Texture alaskanMalamutTexture;
Texture darknessTexture;
Texture jupiterTexture;
Texture uranusTexture;
Texture grassLandTexture;

// Global variables for the scene
glm::mat4 viewProj;
glm::mat4 viewProjInverse;

std::vector<RenderableObject*> sceneObjects;
std::vector<RenderableObjectStatic*> uiObjects;
std::vector<RenderableObject*> animatedBalls;
std::vector<float> ballDirections;


int main() {
    //random seed for number generator
    srand((unsigned int)time(nullptr));

    glfwInit();

    // Seed once before the loop
    srand(static_cast<unsigned int>(time(nullptr)));

#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_DEPTH_BITS, 24); 
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "Project371", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //glClearColor(0.53f, 0.81f, 0.98f, 1.0f);  // Nice sky blue (RGB)

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    Camera camera(aspect);
    RayPicker::getInstance().setCamera(&camera);

    Shader shaderUI(vertexPathUI, fragmentPathUI);
    Shader shader(vertexPath, fragmentPath);
    Shader shaderShadow(vertexPathShadow, fragmentPathShadow);

    std::vector<Tri> tris;

    //triangles for the scene objects
    tris.emplace_back(
        Vertex{{ 0.0f, 1.0f,  0.0f}, {0.2f, 1.0f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{-0.5f, 0.2f,  0.5f}, {0.2f, 1.0f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ 0.5f, 0.2f,  0.5f}, {0.2f, 1.0f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    );

    tris.emplace_back(
        Vertex{{-0.5f,  0.2f,  0.5f}, {1.0f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ 0.0f,  1.0f,  0.0f}, {1.0f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{-0.5f,  0.2f, -0.5f}, {1.0f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}  // Cyan
    );

    tris.emplace_back(
        Vertex{{ 0.0f,  1.0f,  0.0f}, {0.2f, 0.2f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ 0.5f,  0.2f,  0.5f}, {0.2f, 0.2f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ 0.5f,  0.2f, -0.5f}, {0.2f, 0.2f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    );

    // minimalistic floor design to represent the ground
    std::vector<Tri> floor;

    floor.emplace_back(
        Vertex{{ 10.0f, 0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}     // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f, 0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}    // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f,  -10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );
    

    // the UI sidebar maade to place all the components
    std::vector<Tri> sideUI;
    
    sideUI.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ -0.6f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    );

    sideUI.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ -1.0f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    );

    //All possible tiles for the UI sidebar for texture selection on the object
    std::vector<Tri>  tile1 = makeTile(0.2f, 0.1f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri>  tile2 = makeTile(0.09f, -0.01f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri>  tile3 = makeTile(-0.02f, -0.12f, {0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri> grassTiles = makeTile(-0.2f, -0.25f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});


    //All possible tiles for the UI sidebar for texture selection for floor design
    std::vector<LightSource> lights = {
        LightSource(glm::vec3(2.5f, 2.0f, 2.5f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 25.0f),   // Warm light
        LightSource(glm::vec3(-2.5f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 25.0f),   // Warm light
    };

    auto* obj1 = new RenderableObject(tris, &shader, &shaderShadow);
    auto* floorObj = new RenderableObject(floor, &shader, &shaderShadow);
    auto* sidebarObj = new RenderableObjectStatic(sideUI, &shaderUI);

    //_________________________________________________________
    auto* tile1Obj = new RenderableObjectStatic(tile1, &shaderUI);
    auto* tile2Obj = new RenderableObjectStatic(tile2, &shaderUI);
    auto* tile3Obj = new RenderableObjectStatic(tile3, &shaderUI);
    auto* grassTileObj = new RenderableObjectStatic(grassTiles, &shaderUI);


    //since we load the texture is within the constructor, we must initialize after creating the window
    darknessTexture = Texture("textures/darkness.jpg"); 
    alaskanMalamutTexture = Texture("textures/alaskan-malamut.jpg");
    jupiterTexture = Texture("textures/jupiter_surface.jpg");
    uranusTexture = Texture("textures/uranus_surface.jpg");

    grassLandTexture = Texture("textures/grass-texture.jpg");
    
    //Assign textures to the buttons for preview
    tile1Obj->setTexture(&alaskanMalamutTexture);
    tile1Obj->enableTexture(true);
    tile2Obj->setTexture(&darknessTexture);
    tile2Obj->enableTexture(true);
    tile3Obj->setTexture(nullptr); // No texture for the third tile -- assumed this is to remove texture
    tile3Obj->enableTexture(false); 

    //for the floor
    grassTileObj->setTexture(&grassLandTexture);
    grassTileObj->enableTexture(true);


    obj1->setOnClick([&]() {
        std::cout << "Scene Object!\n";
    });
    sidebarObj->setOnClick([&]() {
        std::cout << "SideBar!\n";
    });

    // Assign click handlers for the tiles texture selection
    tile1Obj->setOnClick([obj1]() {
        obj1->setTexture(&alaskanMalamutTexture);
        obj1->enableTexture(true);
        std::cout << "Assigned Alaskan Malamut Texture!\n";
    });

    tile2Obj->setOnClick([obj1]() {
        obj1->setTexture(&darknessTexture);
        obj1->enableTexture(true);
        std::cout << "Assigned Darkness Texture!\n";
    });
    //remove texture selection
    tile3Obj->setOnClick([obj1]() {
        obj1->setTexture(nullptr);
        obj1->enableTexture(false);
        std::cout << "Texture removed!\n";
    });

    grassTileObj->setOnClick([&]() {
        // std::vector<Tri> grassMesh = makeTile(0.05f, -0.05f, {1,1,1}, {0,1}, {1,0});

        // auto grassObjs = spawnPatches(grassMesh, &shader, &shaderShadow, &grassLandTexture, 600);

        // // add to sceneObjects
        // sceneObjects.insert(sceneObjects.end(), grassObjs.begin(), grassObjs.end());
        floorObj->setTexture(&grassLandTexture);
        floorObj->enableTexture(true);
        std::cout << "Spawned grass patches!\n";
    });

    sceneObjects.push_back(obj1);
    //creation of spherical ball objects
    for (int i = 0; i < 10; ++i) {
        float z = -1.0f + (i * 0.8f);
        auto* ball = new RenderableObject(generateSphericalBalls(0.45f, 16, 12), &shader, &shaderShadow);
        ball->setPosition(glm::vec3(-8.0f + i, 0.4f, z));

        // Randomly assign texture: 0 or 1
        int randomNum = rand() % 2; // 0 or 1
        if (randomNum == 0) {
            ball->setTexture(&jupiterTexture);
        } else {
            ball->setTexture(&uranusTexture);
        }
        ball->enableTexture(true);

        sceneObjects.push_back(ball);
        animatedBalls.push_back(ball);
        // Alternate directions: even = +1 (left->right), odd = -1 (right->left)
        ballDirections.push_back((i % 2 == 0) ? 1.0f : -1.0f);
    }


    sceneObjects.push_back(floorObj);
    uiObjects.push_back(sidebarObj);
    //Push each tile objects to UI list of clickable objects
    uiObjects.push_back(tile1Obj);
    uiObjects.push_back(tile2Obj);
    uiObjects.push_back(tile3Obj);
    uiObjects.push_back(grassTileObj);


    std::vector<RenderableObjectBase*> allObjects;
    allObjects.insert(allObjects.end(), sceneObjects.begin(), sceneObjects.end());
    allObjects.insert(allObjects.end(), uiObjects.begin(), uiObjects.end());

    // Mouse click handler expects pointer access
    MouseClickHandler mouseClickHandler(&camera, &allObjects);

    // Set GLFW mouse callback to a lambda that calls your handler
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        // Retrieve the MouseClickHandler pointer stored as GLFW user pointer
        MouseClickHandler* handler = static_cast<MouseClickHandler*>(glfwGetWindowUserPointer(window));
        if (handler)
            handler->handleMouseClick(window, button, action, mods);
    });

    // Set the mouse handler as user pointer so lambda can access it
    glfwSetWindowUserPointer(window, &mouseClickHandler);

    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 

    for (auto& light : lights) {
        light.initShadowCubemap();
    }
    shader.use();
    glDisable(GL_CULL_FACE);
    glm::vec3 cubePos = {0.0f, 0.0f, 0.0f};
    glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos);
    shader.setMat4("uModel", model);
    while (!glfwWindowShouldClose(window)) {
        float deltaTime = computeDeltaTime();
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect = (float)width / height;
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        // 1. For each light, update shadow transforms and render shadow cubemap
        for (LightSource& light : lights) {
            glViewport(0, 0, light.SHADOW_WIDTH, light.SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, light.shadowMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            light.computeShadowTransforms();
            for (int face = 0; face < 6; ++face) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                                    light.shadowCubemap, 0);
                glClear(GL_DEPTH_BUFFER_BIT);

                for (auto* obj : sceneObjects) {
                    obj->drawDepthOnly(light.shadowTransforms[face], light.position, light.farPlane);
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // 2. Render scene normally with all lights and shadows
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.updateKeyControl(deltaTime, window);
        camera.updateProjectionMatrix(aspect);
        viewProj = camera.getViewProjection();
        

        shader.use();
        // Pass number of lights
        glUniform1i(glGetUniformLocation(shader.getID(), "uNumLights"), (int)lights.size());

        // Bind each light's data and shadow map to the shader
        for (int i = 0; i < lights.size(); ++i) {
            const LightSource& light = lights[i];
            std::string prefix = "uLights[" + std::to_string(i) + "]";

            // Light position
            glUniform3fv(glGetUniformLocation(shader.getID(), (prefix + ".position").c_str()), 1, glm::value_ptr(light.position));
            // Light color
            glUniform3fv(glGetUniformLocation(shader.getID(), (prefix + ".color").c_str()), 1, glm::value_ptr(light.color));
            // Intensity
            glUniform1f(glGetUniformLocation(shader.getID(), (prefix + ".intensity").c_str()), light.intensity);
            // Far plane
            glUniform1f(glGetUniformLocation(shader.getID(), (prefix + ".farPlane").c_str()), light.farPlane);

            // Bind shadow cubemap to texture units starting from 1 (or some offset)
            glActiveTexture(GL_TEXTURE1 + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadowCubemap);
            glUniform1i(glGetUniformLocation(shader.getID(), (prefix + ".shadowMap").c_str()), 1 + i);
        }

        // Render all objects
        for (auto* obj : sceneObjects)
            obj->draw(viewProj, lights);

        // Render UI
        for (auto* obj : uiObjects)
            obj->draw(viewProj, lights);


        // Animate balls moving left to right and right-to-left
        // Enable them in zigzag pattern
        for (int i = 0; i < (int)animatedBalls.size(); ++i) {
            glm::vec3 pos = animatedBalls[i]->getPosition();

            pos.x += deltaTime * 1.5f * ballDirections[i]; // move in current direction

            // Check boundaries and flip direction if needed
            if (pos.x > 10.0f) {
                pos.x = 10.0f;           // clamp position at boundary
                ballDirections[i] = -1;  // flip direction to left
            }
            else if (pos.x < -10.0f) {
                pos.x = -10.0f;
                ballDirections[i] = 1;   // flip direction to right
            }

            animatedBalls[i]->setPosition(pos);
        }

        // 7. Events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Exit if ESC pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    for (auto* obj : sceneObjects)
        delete obj;
    for (auto* obj : uiObjects)
        delete obj;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
