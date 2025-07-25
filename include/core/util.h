/**
 * @file util.h
 * @brief Utility functions and constants for the project.
 * This file contains helper functions and constants used throughout the project.
 */
#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
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
/**
 * Helper function to create a UI tile with customizable bounds and UVs.
 */
inline auto makeTile = [](float left, float right, float top, float bottom, float z, glm::vec3 color) -> std::vector<Tri> {
    return {
        Tri{
            Vertex{{left,  top,    z}, color, {0.0f, 1.0f}},
            Vertex{{left,  bottom, z}, color, {0.0f, 0.0f}},
            Vertex{{right, bottom, z}, color, {1.0f, 0.0f}}
        },
        Tri{
            Vertex{{left,  top,    z}, color, {0.0f, 1.0f}},
            Vertex{{right, top,    z}, color, {1.0f, 1.0f}},
            Vertex{{right, bottom, z}, color, {1.0f, 0.0f}}
        }
    };
};



/**
 * generate shperical balls
 */
inline std::vector<Tri> generateSphericalBalls(float radius, int segments, int rings)
{
    std::vector<Tri> tris;
    for (int i = 0; i < rings; ++i) {
        float lat0 = glm::pi<float>() * (-0.5f + (float)(i) / rings);
        float lat1 = glm::pi<float>() * (-0.5f + (float)(i + 1) / rings);
        float y0 = sin(lat0), y1 = sin(lat1);
        float r0 = cos(lat0), r1 = cos(lat1);

        for (int j = 0; j < segments; ++j) {
            float lng0 = 2 * glm::pi<float>() * (float)(j) / segments;
            float lng1 = 2 * glm::pi<float>() * (float)(j + 1) / segments;
            float x0 = cos(lng0), z0 = sin(lng0);
            float x1 = cos(lng1), z1 = sin(lng1);

            glm::vec3 p1 = radius * glm::vec3(x0 * r0, y0, z0 * r0);
            glm::vec3 p2 = radius * glm::vec3(x1 * r0, y0, z1 * r0);
            glm::vec3 p3 = radius * glm::vec3(x1 * r1, y1, z1 * r1);
            glm::vec3 p4 = radius * glm::vec3(x0 * r1, y1, z0 * r1);

            glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);

            // Calculate UVs per vertex for correct texture mapping
            glm::vec2 uv1 = glm::vec2((float)j / segments, (float)i / rings);
            glm::vec2 uv2 = glm::vec2((float)(j + 1) / segments, (float)i / rings);
            glm::vec2 uv3 = glm::vec2((float)(j + 1) / segments, (float)(i + 1) / rings);
            glm::vec2 uv4 = glm::vec2((float)j / segments, (float)(i + 1) / rings);

            tris.emplace_back(Vertex{p1, color, uv1}, Vertex{p2, color, uv2}, Vertex{p3, color, uv3});
            tris.emplace_back(Vertex{p1, color, uv1}, Vertex{p3, color, uv3}, Vertex{p4, color, uv4});
        }
    }
    return tris;
}

/**
 * * @brief Spawns small patches in the scene.
 * * CURRENTLY NOT USED -- COULD BE USEFUL IN THE FUTURE IF WE WISH TO SPAWN A LOT OF OBJECTS
 * * This function generates a specified number of patches at random positions within a defined range.
 */
inline std::vector<RenderableObject*> spawnPatches(
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
