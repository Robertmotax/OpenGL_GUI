// include necessary headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Globals.h"
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

// See Globals.h/.cpp -- ballAngles and radius are connected with animatedBalls
std::vector<float> ballAngles; // angle (radians) for each orbiting ball
std::vector<float> ballRadius; //orbiting the center of the triangle correlated with the original triangle

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
        LightSource(glm::vec3(2.5f, 2.0f, 2.5f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 25.0f),   // Warm light
        LightSource(glm::vec3(-2.5f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 25.0f),   // Warm light
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
        ball->setParent(obj1); // Set initial orbit center to initial original position related to the triangle

        ballAngles.push_back(glm::radians((float)(rand() % 360))); // random start angle
        ballRadius.push_back(3.0f + (i * 0.5f)); // radius of the spherical ball

        sceneObjects.push_back(ball); animatedBalls.push_back(ball);
    }
    sceneObjects.push_back(floorObj);

    sidebar = new Sidebar(&shaderShadow);

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

        // Animate the ball like an orbit solar system
        for (int i = 0; i < (int)animatedBalls.size(); ++i) {
            ballAngles[i] += deltaTime * 0.5f; //0.5f is the speed

            float maxRadius = 9.99f; // ensure it doesnt go past the vicinity of the cube
            ballRadius[i] = std::min(maxRadius, 3.0f + (i * 0.5f));
            // Local orbit transform: rotate then translate by radius
            glm::mat4 orbit = glm::rotate(glm::mat4(1.0f), ballAngles[i], glm::vec3(0, 1, 0));
            orbit = glm::translate(orbit, glm::vec3(ballRadius[i], 0.2f, 0.0f));
            //set new local transforma for children class
            animatedBalls[i]->setLocalTransform(orbit);
        }
        obj1->updateSelfAndChildren();


        // Render all objects
        for (auto* obj : sceneObjects)
            obj->draw(viewProj, lights);

        sidebar->render();

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