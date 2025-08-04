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
#include "core/Model.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Vertex.h"


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

            tris.emplace_back(Vertex{p2, color, uv2}, Vertex{p1, color, uv1}, Vertex{p3, color, uv3});
            tris.emplace_back(Vertex{p3, color, uv3}, Vertex{p1, color, uv1}, Vertex{p4, color, uv4});
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


// Simple ray-AABB test using slab method
inline bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                       const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& tHit) {
    float tMin = 0.0f;
    float tMax = 1e6f;

    for (int i = 0; i < 3; ++i) {
        if (fabs(rayDir[i]) < 1e-6) {
            if (rayOrigin[i] < aabbMin[i] || rayOrigin[i] > aabbMax[i])
                return false;
        } else {
            float ood = 1.0f / rayDir[i];
            float t1 = (aabbMin[i] - rayOrigin[i]) * ood;
            float t2 = (aabbMax[i] - rayOrigin[i]) * ood;
            if (t1 > t2) std::swap(t1, t2);
            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);
            if (tMin > tMax) return false;
        }
    }

    tHit = tMin; //closest
    return true;
}

// Generate a cube RenderableObject
inline std::vector<Tri> makeCube(const glm::vec3& scale, const glm::vec3& color, bool inv = false) {
    std::vector<Tri> cube;

    auto V = [&](glm::vec3 p) -> Vertex {
        glm::vec3 pos = p * scale;
        return Vertex{ pos, color, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
    };

    if(!inv)
    {
        // Front face
        cube.emplace_back(V({-0.5f, -0.5f,  0.5f}), V({ 0.5f, -0.5f,  0.5f}), V({ 0.5f,  0.5f,  0.5f}));
        cube.emplace_back(V({-0.5f, -0.5f,  0.5f}), V({ 0.5f,  0.5f,  0.5f}), V({-0.5f,  0.5f,  0.5f}));

        // Back face
        cube.emplace_back(V({ 0.5f, -0.5f, -0.5f}), V({-0.5f, -0.5f, -0.5f}), V({-0.5f,  0.5f, -0.5f}));
        cube.emplace_back(V({ 0.5f, -0.5f, -0.5f}), V({-0.5f,  0.5f, -0.5f}), V({ 0.5f,  0.5f, -0.5f}));

        // Left face
        cube.emplace_back(V({-0.5f, -0.5f, -0.5f}), V({-0.5f, -0.5f,  0.5f}), V({-0.5f,  0.5f,  0.5f}));
        cube.emplace_back(V({-0.5f, -0.5f, -0.5f}), V({-0.5f,  0.5f,  0.5f}), V({-0.5f,  0.5f, -0.5f}));

        // Right face
        cube.emplace_back(V({ 0.5f, -0.5f,  0.5f}), V({ 0.5f, -0.5f, -0.5f}), V({ 0.5f,  0.5f, -0.5f}));
        cube.emplace_back(V({ 0.5f, -0.5f,  0.5f}), V({ 0.5f,  0.5f, -0.5f}), V({ 0.5f,  0.5f,  0.5f}));

        // Top face
        cube.emplace_back(V({-0.5f,  0.5f,  0.5f}), V({ 0.5f,  0.5f,  0.5f}), V({ 0.5f,  0.5f, -0.5f}));
        cube.emplace_back(V({-0.5f,  0.5f,  0.5f}), V({ 0.5f,  0.5f, -0.5f}), V({-0.5f,  0.5f, -0.5f}));

        // Bottom face
        cube.emplace_back(V({-0.5f, -0.5f, -0.5f}), V({ 0.5f, -0.5f, -0.5f}), V({ 0.5f, -0.5f,  0.5f}));
        cube.emplace_back(V({-0.5f, -0.5f, -0.5f}), V({ 0.5f, -0.5f,  0.5f}), V({-0.5f, -0.5f,  0.5f}));
    }
    else{
        // Front face
        cube.emplace_back(V({ 0.5f, -0.5f,  0.5f}), V({-0.5f, -0.5f,  0.5f}), V({ 0.5f,  0.5f,  0.5f}));
        cube.emplace_back(V({ 0.5f,  0.5f,  0.5f}), V({-0.5f, -0.5f,  0.5f}), V({-0.5f,  0.5f,  0.5f}));

        // Back face
        cube.emplace_back(V({-0.5f, -0.5f, -0.5f}), V({ 0.5f, -0.5f, -0.5f}), V({-0.5f,  0.5f, -0.5f}));
        cube.emplace_back(V({-0.5f,  0.5f, -0.5f}), V({ 0.5f, -0.5f, -0.5f}), V({ 0.5f,  0.5f, -0.5f}));

        // Left face
        cube.emplace_back(V({-0.5f, -0.5f,  0.5f}), V({-0.5f, -0.5f, -0.5f}), V({-0.5f,  0.5f,  0.5f}));
        cube.emplace_back(V({-0.5f,  0.5f,  0.5f}), V({-0.5f, -0.5f, -0.5f}), V({-0.5f,  0.5f, -0.5f}));

        // Right face
        cube.emplace_back(V({ 0.5f, -0.5f, -0.5f}), V({ 0.5f, -0.5f,  0.5f}),  V({ 0.5f,  0.5f, -0.5f}));
        cube.emplace_back(V({ 0.5f,  0.5f, -0.5f}), V({ 0.5f, -0.5f,  0.5f}), V({ 0.5f,  0.5f,  0.5f}));

        // Top face
        cube.emplace_back(V({ 0.5f,  0.5f,  0.5f}), V({-0.5f,  0.5f,  0.5f}),  V({ 0.5f,  0.5f, -0.5f}));
        cube.emplace_back(V({ 0.5f,  0.5f, -0.5f}), V({-0.5f,  0.5f,  0.5f}), V({-0.5f,  0.5f, -0.5f}));

        // Bottom face
        cube.emplace_back(V({ 0.5f, -0.5f, -0.5f}), V({-0.5f, -0.5f, -0.5f}), V({ 0.5f, -0.5f,  0.5f}));
        cube.emplace_back(V({ 0.5f, -0.5f,  0.5f}), V({-0.5f, -0.5f, -0.5f}), V({-0.5f, -0.5f,  0.5f}));
    }

    return cube;
}


inline std::vector<Tri> objToTri(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return {};
    }

    std::vector<glm::vec3> positions;
    std::vector<Tri> tris;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            positions.emplace_back(x, y, z);
        } else if (type == "f") {
            int idx[3];
            for (int i = 0; i < 3; ++i) {
                std::string token;
                ss >> token;
                std::stringstream tokenStream(token);
                std::string indexStr;
                std::getline(tokenStream, indexStr, '/');
                idx[i] = std::stoi(indexStr) - 1; // OBJ indices are 1-based
            }

            // Build Tri from positions and default color/texCoord/normal
            Vertex v0{ positions[idx[0]], {0.7f, 0.7f, 0.7f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} };
            Vertex v1{ positions[idx[1]], {0.7f, 0.7f, 0.7f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} };
            Vertex v2{ positions[idx[2]], {0.7f, 0.7f, 0.7f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} };
            tris.emplace_back(v0, v1, v2);
        }
    }

    return tris;
}
