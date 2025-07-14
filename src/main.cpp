#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/Shader.h"
#include "core/RenderableObject.h"
#include "core/RenderableObjectNDC.h"
#include "core/Tri.h"
#include "core/Camera.h"
#include "core/MouseClickHandler.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

const char* vertexPath = "shaders/vertex.glsl";
const char* fragmentPath = "shaders/fragment.glsl";

glm::mat4 viewProj;
glm::mat4 viewProjInverse;

std::vector<RenderableObjectBase*> allObjects;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        for (auto& obj : allObjects) {
            // pass the global viewProjInverse here
            if (obj->isClicked((float)xpos, (float)ypos, width, height, viewProjInverse)) {
                obj->onClick();
            }
        }
    }
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
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

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

    Shader shaderNDC(vertexPath, fragmentPath, true);
    Shader shader(vertexPath, fragmentPath, false);


    std::vector<Tri> tris;

    tris.emplace_back(
        Vertex{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Red
        Vertex{{ 0.0f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Green
        Vertex{{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}   // Blue
    );

    tris.emplace_back(
        Vertex{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Red
        Vertex{{ 0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Green
        Vertex{{-0.5f,  -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}   // Cyan
    );

    tris.emplace_back(
        Vertex{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},   // Blue
        Vertex{{ 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Green
        Vertex{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}
    );

    std::vector<Tri> sideUI;

    sideUI.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}},
        Vertex{{ -0.6f,  -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}}
    );

    sideUI.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}},
        Vertex{{ -1.0f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}}
    );

    auto* obj1 = new RenderableObject(tris, &shader);
    auto* obj2 = new RenderableObjectNDC(sideUI, &shaderNDC);

    obj1->setOnClick([]() {
        std::cout << "Scene Object!\n";
    });
    obj2->setOnClick([]() {
        std::cout << "SideBar!\n";
    });

    allObjects.push_back(obj1);
    allObjects.push_back(obj2);

    
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

    float lastTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwGetWindowSize(window, &width, &height);
        float aspect = (float)width / (float)height;
        camera.updateProjectionMatrix(aspect);

        shader.use();
        glm::mat4 vp = camera.getViewProjection();
        glUniformMatrix4fv(
            glGetUniformLocation(shader.getID(), "uVP"),
            1, GL_FALSE,
            &vp[0][0]
        );

        for (auto* obj : allObjects) {
            obj->draw(vp);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera.update(deltaTime, window);


        // update viewProjInverse globally or inside camera class
        viewProjInverse = glm::inverse(vp);
    }

    for (auto* obj : allObjects)
        delete obj;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
