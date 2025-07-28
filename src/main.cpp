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
#include <singleton/RayPicker.h>
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
#include <functional>

// This library used for texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#include <ui/Sidebar.h>


const char* vertexPath = "shaders/main.vert";
const char* fragmentPath = "shaders/main.frag";
const char* vertexPathShadow = "shaders/shadow.vert";
const char* fragmentPathShadow = "shaders/shadow.frag";

// Global variables for the scene
glm::mat4 viewProj;
glm::mat4 viewProjInverse;

std::vector<RenderableObject*> sceneObjects;
std::vector<RenderableObject*> animatedBalls;
std::vector<float> ballDirections;
Sidebar *sidebar;


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

    Camera camera(aspect);
    RayPicker::getInstance().setCamera(&camera);

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

    tris.emplace_back(
        Vertex{{ 0.0f,  1.0f,  0.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{-0.5f,  0.2f, -0.5f}, {0.2f, 0.2f, 0.2f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        Vertex{{ 0.5f,  0.2f, -0.5f}, {0.3f, 0.2f, 0.2f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
    );

    // minimalistic floor design to represent the ground
    //requires 12 triangles for 6 squares for 36 vertices
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

    //All possible tiles for the UI sidebar for texture selection for floor design
    std::vector<LightSource> lights = {
        LightSource(glm::vec3(2.5f, 2.0f, 2.5f), glm::vec3(1.0f, 0.8f, 0.6f), glm::vec3(0.5f), &shader, &shaderShadow),
        LightSource(glm::vec3(-2.5f, 2.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.7f), &shader, &shaderShadow)
    };


    auto* obj1 = new RenderableObject(tris, &shader, &shaderShadow);
    auto* floorObj = new RenderableObject(floor, &shader, &shaderShadow);
    obj1->setName("Prism");
    floorObj->setName("BigCube");

    sceneObjects.push_back(obj1);
    //creation of spherical ball objects
    for (int i = 0; i < 10; ++i) {
        float z = -1.0f + (i * 0.8f);
        auto* ball = new RenderableObject(generateSphericalBalls(0.45f, 16, 12), &shader, &shaderShadow);
        ball->setName("ball " + std::to_string(i));
        ball->setPosition(glm::vec3(-8.0f + i, 0.4f, z));

        sceneObjects.push_back(ball);
        animatedBalls.push_back(ball);
        // Alternate directions: even = +1 (left->right), odd = -1 (right->left)
        ballDirections.push_back((i % 2 == 0) ? 1.0f : -1.0f);
    }
    sceneObjects.push_back(floorObj);

    sidebar = new Sidebar();

    std::vector<RenderableObjectBase*> allObjects;
    allObjects.insert(allObjects.end(), sceneObjects.begin(), sceneObjects.end());
    allObjects.insert(allObjects.end(), sidebar->uiElements.begin(), sidebar->uiElements.end());

    // Mouse click handler expects pointer access
    MouseClickHandler mouseClickHandler(&camera, &allObjects);

    // Set GLFW mouse callback to a lambda that calls your handler
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        // Retrieve the MouseClickHandler pointer stored as GLFW user pointer
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

    for (auto& light : lights) {
        light.initShadowCubemap();
    }
    shader.use();
    glDisable(GL_CULL_FACE);
    glm::vec3 cubePos = {0.0f, 0.0f, 0.0f};
    glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos);
    shader.setMat4("uModel", model);

    for(RenderableObject*obj : sceneObjects)
    {
        obj->setOnClick([obj]() {
            sidebar->setSelectedObject(obj);
        });
    }
    for (LightSource& light : lights)
    {
        RenderableObject* handler = light.lightHandler;
        light.lightHandler->setOnClick([handler]() {
            sidebar->setSelectedObject(handler);
        });
        allObjects.push_back(handler);
    }
    while (!glfwWindowShouldClose(window)) {
        float deltaTime = computeDeltaTime();
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect = (float)width / height;
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        // 1. For each light, update shadow transforms and render shadow cubemap
        for (LightSource& light : lights) {
            light.renderShadowMap(sceneObjects);
        }

        // 2. Render scene normally with all lights and shadows
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.updateKeyControl(deltaTime, window);
        camera.updateProjectionMatrix(aspect);
        viewProj = camera.getViewProjection();

        // Render all objects
        for (auto* obj : sceneObjects)
            obj->draw(viewProj, lights);
        for (LightSource& light : lights)
            light.lightHandler->draw(viewProj, {});
        sidebar->render();

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
    for (auto* obj : sidebar->uiElements)
        delete obj;
    delete sidebar;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}