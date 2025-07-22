/**
 * @file util.h
 * @brief Utility functions and constants for the project.
 * This file contains helper functions and constants used throughout the project.
 */
#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <GL/glew.h>
#include "Vertex.h"
#include "Tri.h"
#include "Shader.h"
#include "Texture.h"
#include "LightSource.h"
#include "RenderableObject.h"
#include <iostream>
#include <vector>


/**
 * * @brief Computes the time difference since the last frame.
 * * This function calculates the time elapsed since the last call, which can be used for frame rate independent movement.
 * * It uses GLFW's time functions to get the current time and compute the delta time.
 */
inline float computeDeltaTime() {
    static float lastTime = glfwGetTime();
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    return deltaTime;
}


/**
 * * Helper function to create a tile for the UI sidebar.
 */
inline auto makeTile = [](float top, float bottom, glm::vec3 color, glm::vec2 uvTopLeft, glm::vec2 uvBottomRight) -> std::vector<Tri> {
    return {
        // Create two triangles for the tile
        // Each triangle is defined by three vertices
        // Each triangle is composed of its position, color, and texture coordinates
        // First triangle

        {   
            Vertex{{-0.6f, top, 0.0f}, color, uvTopLeft},
            Vertex{{-0.6f, bottom, 0.0f}, color, {uvTopLeft.x, uvBottomRight.y}},
            Vertex{{-1.0f, bottom, 0.0f}, color, uvBottomRight}
        },
        // Second triangle
        {
            Vertex{{-0.6f, top, 0.0f}, color, uvTopLeft},
            Vertex{{-1.0f, top, 0.0f}, color, {uvBottomRight.x, uvTopLeft.y}},
            Vertex{{-1.0f, bottom, 0.0f}, color, uvBottomRight}
        }
    };
};

/**
 * * @brief Spawns small patches in the scene.
 * * This function generates a specified number of patches at random positions within a defined range.
 */
std::vector<RenderableObject*> spawnPatches(
    const std::vector<Tri>& Mesh, 
    Shader* shader, Shader* shadowShader, 
    Texture* texture,
    int count = 300)
{
    std::vector<RenderableObject*> grassList;

    for (int i = 0; i < count; ++i) {
        float x = ((rand() % 10000) / 10000.0f) * 100.0f - 50.0f;
        float z = ((rand() % 10000) / 10000.0f) * 100.0f - 50.0f;

        float angle = ((rand() % 10000) / 10000.0f) * 360.0f;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));

        auto* grassObj = new RenderableObject(Mesh, shader, shadowShader);
        grassObj->setTexture(texture);
        grassObj->enableTexture(true);
        grassObj->setModelMatrix(model);

        grassList.push_back(grassObj);
    }
    return grassList;
}
