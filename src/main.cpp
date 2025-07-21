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


const char* vertexPath = "shaders/vertex.glsl";
const char* fragmentPath = "shaders/fragment.glsl";
const char* vertexPathUI = "shaders/vertexUI.glsl";
const char* fragmentPathUI = "shaders/fragmentUI.glsl";
const char* vertexPathShadow = "shaders/vertexShadow.glsl";
const char* fragmentPathShadow = "shaders/fragmentShadow.glsl";

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
        Vertex{{-5.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f}, {0.0f, 1.0f}},  // Red
        Vertex{{ 0.0f, 1.0f, 0.0f}, {0.0f, 0.5f, 0.0f}, {0.0f, 0.0f}},  // Green
        Vertex{{ 0.5f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f}, {1.0f, 1.0f}}   // Blue
    );

    tris.emplace_back(
        Vertex{{-5.0f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f},   {0.0f, 1.0f}},  // Red
        Vertex{{ 0.0f,  1.0f, 0.0f}, {0.5f, 0.0f, 0.0f},  {0.0f, 0.0f}},  // Green
        Vertex{{-0.5f,  0.0f, -5.0f}, {0.5f, 0.0f, 0.0f}, {1.0f, 1.0f}}   // Cyan
    );

    tris.emplace_back(
        Vertex{{ 0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.5f},   {0.0f, 1.0f}},   // Blue
        Vertex{{ 0.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.5f},  {0.0f, 0.0f}},  // Green
        Vertex{{ 0.5f, 0.0f, -10.0f}, {0.0f, 0.0f, 0.5f}, {1.0f, 1.0f}}
    );

    // minimalistic floor design to represent the ground
    std::vector<Tri> floor;

    floor.emplace_back(
        Vertex{{-50.0f, 0.0f,  50.0f}, {0.2f, 0.2f, 0.2f}},   // Dark Gray
        Vertex{{ 50.0f, 0.0f, -50.0f}, {0.2f, 0.2f, 0.2f}},   // Dark Gray
        Vertex{{ 50.0f, 0.0f,  50.0f}, {0.2f, 0.2f, 0.2f}}    // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-50.0f, 0.0f,  50.0f}, {0.2f, 0.2f, 0.2f}},   // Dark Gray
        Vertex{{ 50.0f, 0.0f, -50.0f}, {0.2f, 0.2f, 0.2f}},   // Dark Gray
        Vertex{{-50.0f, 0.0f, -50.0f}, {0.2f, 0.2f, 0.2f}}   // Dark Gray
    );

    // the UI sidebar maade to place all the components
    std::vector<Tri> sideUI;
    
    sideUI.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}},
        Vertex{{ -0.6f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f}}
    );

    sideUI.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}},
        Vertex{{ -1.0f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 1.0f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f}}
    );

    //All possible tiles for the UI sidebar
    std::vector<Tri>  tile1 = makeTile(0.1f, -0.2f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri>  tile2 = makeTile(-0.25f, -0.55f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri>  tile3 = makeTile(-0.6f, -0.9f, {0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f});


    std::vector<LightSource> lights = {
        LightSource(glm::vec3(1.0f, 1.0f, 2.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f),   // Warm light
        LightSource(glm::vec3(-1.5f, 1.0f, 1.5f), glm::vec3(0.4f, 0.7f, 1.0f), 0.7f)   // Cool light
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

    while (!glfwWindowShouldClose(window)) {
        // 1. Time and window size
        float deltaTime = computeDeltaTime(); // Your own time logic
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect = (float)width / height;

        // 2. Update camera (position, rotation, etc.)
        camera.updateKeyControl(deltaTime, window);
        camera.updateProjectionMatrix(aspect); // If aspect changes


        // 3. Recalculate matrices (important!)
        glm::mat4 viewProj = camera.getViewProjection();
        viewProjInverse = glm::inverse(viewProj);

        for (auto& light : lights) {
            glm::mat4 lightView = glm::lookAt(light.position, glm::vec3(0), glm::vec3(0, 1, 0));
            glm::mat4 lightProj = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 20.f);
            light.lightSpaceMatrix = lightProj * lightView;

            glViewport(0, 0, 1024, 1024);
            glBindFramebuffer(GL_FRAMEBUFFER, light.shadowMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            shaderShadow.use();
            glUniformMatrix4fv(glGetUniformLocation(shaderShadow.getID(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));

            for (auto* obj : sceneObjects)
                obj->drawDepthOnly();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // 5. Clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 6. Render
        for (auto* obj : sceneObjects)
            obj->draw(viewProj, lights);

        for (auto* obj : uiObjects)
            obj->draw(viewProj, {});  // No lights needed

        // 7. Events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        // Close window if pressed escape
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
