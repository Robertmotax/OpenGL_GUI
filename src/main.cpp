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
#include <cstdlib> 
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// This library used for texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#include <ui/Sidebar.h>


const char* vertexPath = "shaders/vertex.glsl";
const char* fragmentPath = "shaders/fragment.glsl";
const char* vertexPathShadow = "shaders/vertexShadow.glsl";
const char* fragmentPathShadow = "shaders/fragmentShadow.glsl";

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
std::vector<RenderableObject*> animatedBalls;
std::vector<float> ballDirections;
Sidebar *sidebar = new Sidebar();

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
        Vertex{{-50.0f, 0.0f,  50.0f}, {0.2f, 0.2f, 0.2f},   {0.0f, 1.0f}},   // Dark Gray
        Vertex{{ 50.0f, 0.0f, -50.0f}, {0.2f, 0.2f, 0.2f},   {0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 50.0f, 0.0f,  50.0f}, {0.2f, 0.2f, 0.2f},   {1.0f, 1.0f}}    // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-50.0f, 0.0f,  50.0f}, {0.2f, 0.2f, 0.2f},   {0.0f, 1.0f}},   // Dark Gray
        Vertex{{ 50.0f, 0.0f, -50.0f}, {0.2f, 0.2f, 0.2f},   {0.0f, 0.0f}},   // Dark Gray
        Vertex{{-50.0f, 0.0f, -50.0f}, {0.2f, 0.2f, 0.2f},   {1.0f, 1.0f}}   // Dark Gray
    );


    //All possible tiles for the UI sidebar for texture selection on the object
    std::vector<Tri>  tile1 = makeTile(0.2f, 0.1f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri>  tile2 = makeTile(0.09f, -0.01f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri>  tile3 = makeTile(-0.02f, -0.12f, {0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f});
    std::vector<Tri> grassTiles = makeTile(-0.2f, -0.25f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});


    //All possible tiles for the UI sidebar for texture selection for floor design
    std::vector<LightSource> lights = {
        LightSource(glm::vec3(1.0f, 1.0f, 2.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f),   // Warm light
        LightSource(glm::vec3(-1.5f, 1.0f, 1.5f), glm::vec3(0.4f, 0.7f, 1.0f), 0.7f)   // Cool light
    };

    auto* obj1 = new RenderableObject(tris, &shader, &shaderShadow);
    auto* floorObj = new RenderableObject(floor, &shader, &shaderShadow);


    //since we load the texture is within the constructor, we must initialize after creating the window
    darknessTexture = Texture("textures/darkness.jpg"); 
    alaskanMalamutTexture = Texture("textures/alaskan-malamut.jpg");
    jupiterTexture = Texture("textures/jupiter_surface.jpg");
    uranusTexture = Texture("textures/uranus_surface.jpg");

    grassLandTexture = Texture("textures/grass-texture.jpg");


    obj1->setOnClick([&]() {
        std::cout << "Scene Object!\n";
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

        // Handle inputs
        // Close window if pressed escape
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
