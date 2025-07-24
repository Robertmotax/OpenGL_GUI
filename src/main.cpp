// include necessary headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f,  -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{-10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f, 10.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f, -10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
    );

    floor.emplace_back(
        Vertex{{ 10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{-10.0f, 10.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},   // Dark Gray
        Vertex{{ 10.0f,  0.0f,  10.0f}, {0.2f, 0.2f, 0.2f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}   // Dark Gray
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

    std::vector<LightSource> lights = {
        LightSource(glm::vec3(2.5f, 2.0f, 2.5f), glm::normalize(glm::vec3(-1.0f, 0.0f, -1.0f)), glm::vec3(1.0f, 0.8f, 0.6f), 0.75f),   // Warm light
    };

    auto* obj1 = new RenderableObject(tris, &shader, &shaderShadow);
    auto* floorObj = new RenderableObject(floor, &shader, &shaderShadow);
    auto* sidebarObj = new RenderableObjectStatic(sideUI, &shaderUI);

    // @TODO --REMOVE: Set textures for objects JUST TO TRY IT OUT
    //since we load the texture is within the constructor, we must initialize after creating the window
    darknessTexture = Texture("textures/darkness.jpg"); 
    sidebarObj->setTexture(&darknessTexture);
    sidebarObj->enableTexture(true);


    obj1->setOnClick([]() {
        std::cout << "Scene Object!\n";
    });
    sidebarObj->setOnClick([]() {
        std::cout << "SideBar!\n";
    });

    sceneObjects.push_back(obj1);
    sceneObjects.push_back(floorObj);
    uiObjects.push_back(sidebarObj);

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
        light.initShadowMap();
    }

    glDisable(GL_CULL_FACE);
    glm::vec3 cubePos = {0.0f, 0.0f, 0.0f};
    glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos);
    shader.setMat4("model", model);
    while (!glfwWindowShouldClose(window)) {
        // 1. Time and window size
        float deltaTime = computeDeltaTime(); // Your own time logic
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect = (float)width / height;

        // 2. Update camera (position, rotation, etc.)
        camera.updateKeyControl(deltaTime, window);
        camera.updateProjectionMatrix(aspect); // If aspect changes

        
        glEnable(GL_DEPTH_TEST);
        for (int i = 0; i < lights.size(); ++i) {
            lights[i].computeLightSpaceMatrix();

            glViewport(0, 0, lights[i].SHADOW_WIDTH, lights[i].SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, lights[i].shadowMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            for (auto* obj : sceneObjects)
                obj->drawDepthOnly(lights[i].lightSpaceMatrix);  // <<--- Pass the matrix

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        // 5. Clear
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Optional but helpful
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // Bind shadow maps for sampling (IMPORTANT: do it here, not above)
        for (int i = 0; i < lights.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, lights[i].shadowMapTex);
            shader.setInt(("uShadowMaps[" + std::to_string(i) + "]").c_str(), i);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR::FRAMEBUFFER:: Shadow map framebuffer not complete!\n";
        }

        // Set light uniforms, camera, etc. here
        for (int i = 0; i < lights.size(); ++i) {
            shader.setVec3(("lightPositions[" + std::to_string(i) + "]").c_str(), lights[i].position);
            shader.setVec3(("lightColors[" + std::to_string(i) + "]").c_str(), lights[i].color);
            shader.setFloat(("lightIntensities[" + std::to_string(i) + "]").c_str(), lights[i].intensity);
            shader.setMat4(("lightSpaceMatrices[" + std::to_string(i) + "]").c_str(), lights[i].lightSpaceMatrix);
        }
        shader.setInt("uNumLights", (int)lights.size());

        // 6. Render
        viewProj = camera.getViewProjection();
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
