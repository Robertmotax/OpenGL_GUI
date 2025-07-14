#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/Shader.h"
#include "core/RenderableObject.h"
#include "core/RenderableObjectNDC.h"
#include "core/Tri.h"
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
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f); // Z = 1 for orthographic

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        for (auto& obj : allObjects) {
            if (obj->isClicked(xpos, ypos, width, height)) {
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

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f), // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f), // Target position
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),         // Field of view
        (float)width / (float)windowHeight, // Aspect ratio
        0.1f,                        // Near clipping plane
        100.0f                       // Far clipping plane
    );

glm::mat4 viewProj = projection * view;

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
        Vertex{{-0.4f,  0.2f, 0.0f}, {1.0f, 0.0f, 0.0f}}   // Cyan
    );

    tris.emplace_back(
        Vertex{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},   // Blue
        Vertex{{ 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Green
        Vertex{{ 0.8f, -0.1f, 0.0f}, {0.0f, 0.0f, 1.0f}}
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
        std::cout << "Triangle 1 clicked!\n";
    });
    obj2->setOnClick([]() {
        std::cout << "Triangle 2 clicked!\n";
    });

    allObjects.push_back(obj1);
    allObjects.push_back(obj2);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect = (float)width / (float)height;

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, 0.1f, 10.0f);
        glm::mat4 VP = projection * view;

        shader.use();
        glUniformMatrix4fv(
            glGetUniformLocation(shader.getID(), "uVP"),
            1, GL_FALSE,
            &viewProj[0][0]
        );

        for (auto* obj : allObjects) {
            obj->draw(viewProj);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        float cameraSpeed = 0.01f;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cameraPos.x -= cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cameraPos.x += cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            cameraPos.y += cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            cameraPos.y -= cameraSpeed;
    }

    for (auto* obj : allObjects)
        delete obj;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
