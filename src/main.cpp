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
#include "core/Texture.h"
#include "core/MouseClickHandler.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

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


glm::mat4 viewProj;
glm::mat4 viewProjInverse;

std::vector<RenderableObject*> sceneObjects;
std::vector<RenderableObjectStatic*> uiObjects;

inline float computeDeltaTime() {
    static float lastTime = glfwGetTime();
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    return deltaTime;
}

int main() {
    glfwInit();

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

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    Camera camera(aspect);

    Shader shaderUI(vertexPathUI, fragmentPathUI);
    Shader shader(vertexPath, fragmentPath);
    Shader shaderShadow(vertexPathShadow, fragmentPathShadow);

    std::vector<Tri> tris;

    //triangles for the scene objects
    tris.emplace_back(
        Vertex{{ 0.0f, 1.0f,  0.0f}, {0.2f, 1.0f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{-0.5f, 0.2f,  0.5f}, {0.2f, 1.0f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ 0.5f, 0.2f,  0.5f}, {0.2f, 1.0f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    );

    tris.emplace_back(
        Vertex{{ 0.0f,  1.0f,  0.0f}, {1.0f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{-0.5f,  0.2f,  0.5f}, {1.0f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{-0.5f,  0.2f, -0.5f}, {1.0f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}  // Cyan
    );

    tris.emplace_back(
        Vertex{{ 0.0f,  1.0f,  0.0f}, {0.2f, 0.2f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ 0.5f,  0.2f,  0.5f}, {0.2f, 0.2f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ 0.5f,  0.2f, -0.5f}, {0.2f, 0.2f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    );

    // minimalistic floor design to represent the ground
    std::vector<Tri> floor;

    floor.emplace_back(
        Vertex{{ 10.0f, 0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}     // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f, 0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}    // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f,  -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );
    

    // the UI sidebar maade to place all the components
    std::vector<Tri> sideUI;
    
    sideUI.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ -0.6f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    );

    sideUI.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ -1.0f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    );

    //All possible tiles for the UI sidebar
    std::vector<Tri>  tile1 = makeTile(0.1f, -0.2f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri>  tile2 = makeTile(-0.25f, -0.55f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri>  tile3 = makeTile(-0.6f, -0.9f, {0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f});


    std::vector<LightSource> lights = {
        LightSource(glm::vec3(2.5f, 2.0f, 2.5f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 25.0f),   // Warm light
    };

    auto* obj1 = new RenderableObject(tris, &shader, &shaderShadow);
    auto* floorObj = new RenderableObject(floor, &shader, &shaderShadow);
    auto* sidebarObj = new RenderableObjectStatic(sideUI, &shaderUI);

    //_________________________________________________________
    auto* tile1Obj = new RenderableObjectStatic(tile1, &shaderUI);
    auto* tile2Obj = new RenderableObjectStatic(tile2, &shaderUI);
    auto* tile3Obj = new RenderableObjectStatic(tile3, &shaderUI);

    //since we load the texture is within the constructor, we must initialize after creating the window
    darknessTexture = Texture("textures/darkness.jpg"); 
    alaskanMalamutTexture = Texture("textures/alaskan-malamut.jpg");

    //Assign textures to the buttons for preview
    tile1Obj->setTexture(&alaskanMalamutTexture);
    tile1Obj->enableTexture(true);

    tile2Obj->setTexture(&darknessTexture);
    tile2Obj->enableTexture(true);

    tile3Obj->setTexture(nullptr); // No texture for the third tile -- assumed this is to remove texture
    tile3Obj->enableTexture(false); 


    obj1->setOnClick([]() {
        std::cout << "Scene Object!\n";
    });
    sidebarObj->setOnClick([]() {
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

    sceneObjects.push_back(obj1);
    sceneObjects.push_back(floorObj);
    uiObjects.push_back(sidebarObj);
    //Push each tile objects to UI list of clickable objects
    uiObjects.push_back(tile1Obj);
    uiObjects.push_back(tile2Obj);
    uiObjects.push_back(tile3Obj);

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
