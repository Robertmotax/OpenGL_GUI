// include necessary headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Globals.h"
#include <ui/Sidebar.h>
#include "core/util.h"
#include "core/Shader.h"
#include "core/RenderableObject.h"
#include "core/RenderableObjectStatic.h"
#include "core/LightSource.h"
#include "core/Tri.h"
#include "core/Camera.h"
#include <singleton/RayPicker.h>
#include "core/Texture.h"
#include "core/MouseClickHandler.h"
#include "core/Model.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib> 
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <functional>
#include "core/OBJLoader.h"

/** External files used for fast modular help */
// This library used for texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"


const char* vertexPath = "shaders/main.vert";
const char* fragmentPath = "shaders/main.frag";
const char* vertexPathShadow = "shaders/shadow.vert";
const char* fragmentPathShadow = "shaders/shadow.frag";
const char* humanModelPath = "models/human.obj";

// Global variables for the scene
glm::mat4 viewProj;
glm::mat4 viewProjInverse;

// See Globals.h/.cpp -- ballAngles and radius are connected with animatedBalls
std::vector<float> ballAngles; // angle (radians) for each orbiting ball
std::vector<float> ballRadius; //orbiting the center of the triangle correlated with the original triangle

Sidebar *sidebar;


int main() {
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

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
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
    OBJLoader objLoader = OBJLoader();

    Camera camera(aspect);
    RayPicker::getInstance().setCamera(&camera);

    defaultShader = new Shader(vertexPath, fragmentPath);
    shadowShader = new Shader(vertexPathShadow, fragmentPathShadow);
    //_____________________________________________________________________
    //objLoader.importFromObj("assets/models/testScene.obj");
    sidebar = new Sidebar();
    //______________________________________________________________________

    for (const auto& ui : sidebar->uiElements) {
        allObjects.push_back(ui.object);
    }
    
    // Mouse click handler expects pointer access
    MouseClickHandler mouseClickHandler(&camera, &allObjects);

    // Set GLFW mouse callback to a lambda that calls your handler
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        MouseClickHandler* handler = static_cast<MouseClickHandler*>(glfwGetWindowUserPointer(window));
        if (handler)
            handler->handleMouseClick(window, button, action, mods);
    });
    // Set mouse cursor callback to a lambda that calls your handler
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        MouseClickHandler* mouseHandler = static_cast<MouseClickHandler*>(glfwGetWindowUserPointer(window));
        if (mouseHandler)
            mouseHandler->handleMouseMove(window, xpos, ypos);
    });

    // Set the mouse handler as user pointer so lambda can access it
    glfwSetWindowUserPointer(window, &mouseClickHandler);

    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    defaultShader->use();
    glDisable(GL_CULL_FACE);
    glm::vec3 cubePos = {0.0f, 0.0f, 0.0f};
    glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos);
    defaultShader->setMat4("uModel", model);

    while (!glfwWindowShouldClose(window)) {
        float deltaTime = computeDeltaTime();
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect = (float)width / height;
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        // 1. For each light, update shadow transforms and render shadow cubemap
        for (LightSource* light : lights) {
            light->renderShadowMap();
        }

        // 2. Render scene normally with all lights and shadows
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.updateKeyControl(deltaTime, window);
        camera.updateProjectionMatrix(aspect);
        viewProj = camera.getViewProjection();

        // Render all objects
        for (auto* obj : allObjects)
        {
            if(dynamic_cast<RenderableObject*>(obj))
            {
                obj->draw(viewProj, lights);
            }
        }
        for (LightSource* light : lights)
            light->lightHandler->draw(viewProj, {});  

        sidebar->updateVisibility(window);
        sidebar->render();

        // 7. Events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Exit if ESC pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    for (auto* obj : allObjects)
        delete obj;
    delete sidebar;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}