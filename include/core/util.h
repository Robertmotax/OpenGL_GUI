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

inline std::vector<Tri> makeCube(const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& color, bool inv = false) {
    constexpr float texWidth = 192.0f;
    constexpr float texHeight = 256.0f;
    constexpr float tileSize = 64.0f;

    auto mapUV = [&](float px, float py, glm::vec2 local) {
        float flipped_py = texHeight - py - tileSize;
        float u = (px + local.x * tileSize) / texWidth;
        float v = (flipped_py + local.y * tileSize) / texHeight;
        u = std::round(u * 3) / 3.0f;
        v = std::round(v * 4) / 4.0f;
        return glm::vec2(u, v);
    };

    // Helper: Vertex with scaled position and UV from atlas
    auto V = [&](glm::vec3 p, float px, float py, glm::vec2 local) -> Vertex {
        glm::vec3 pos_scaled = p * scale;
        glm::vec3 pos_final = pos_scaled + pos;
        return Vertex{ pos_final, color, mapUV(px, py, local), {0.0f, 0.0f, 0.0f} };
    };

    std::vector<Tri> cube;

    // Face UV origins in pixels
    float R_x=128,  R_y=64;
    float T_x=64,   T_y=0;
    float F_x=64,  F_y=64;
    float Bm_x=64,Bm_y=128;
    float Ba_x=64,  Ba_y=196;
    float L_x=0,  L_y=64;

    auto addFace = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float fx, float fy) {
        // p0-p1-p2, p0-p2-p3  (counter-clockwise)
        cube.emplace_back(
            V(p0, fx, fy, {0,0}),
            V(p1, fx, fy, {1,0}),
            V(p2, fx, fy, {1,1})
        );
        cube.emplace_back(
            V(p0, fx, fy, {0,0}),
            V(p2, fx, fy, {1,1}),
            V(p3, fx, fy, {0,1})
        );
    };

    // Standard cube positions
    glm::vec3 p000 = {-0.5f, -0.5f, -0.5f};
    glm::vec3 p001 = {-0.5f, -0.5f,  0.5f};
    glm::vec3 p010 = {-0.5f,  0.5f, -0.5f};
    glm::vec3 p011 = {-0.5f,  0.5f,  0.5f};
    glm::vec3 p100 = { 0.5f, -0.5f, -0.5f};
    glm::vec3 p101 = { 0.5f, -0.5f,  0.5f};
    glm::vec3 p110 = { 0.5f,  0.5f, -0.5f};
    glm::vec3 p111 = { 0.5f,  0.5f,  0.5f};

    if (!inv) {
        addFace(p001, p101, p111, p011, F_x, F_y); // Back
        addFace(p100, p000, p010, p110, Ba_x, Ba_y); // Front
        addFace(p000, p001, p011, p010, L_x, L_y); // Left
        addFace(p101, p100, p110, p111, R_x, R_y); // Right
        addFace(p011, p111, p110, p010, T_x, T_y); // Top
        addFace(p000, p100, p101, p001, Bm_x, Bm_y); // Bottom
    } else {
        // Inverted winding order
        auto addFaceInv = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float fx, float fy) {
            cube.emplace_back(
                V(p0, fx, fy, {0,0}),
                V(p2, fx, fy, {1,1}),
                V(p1, fx, fy, {1,0})
            );
            cube.emplace_back(
                V(p0, fx, fy, {0,0}),
                V(p3, fx, fy, {0,1}),
                V(p2, fx, fy, {1,1})
            );
        };
        addFaceInv(p001, p101, p111, p011, F_x, F_y); // Back
        addFaceInv(p100, p000, p010, p110, Ba_x, Ba_y); // Front
        addFaceInv(p000, p001, p011, p010, L_x, L_y); // Left
        addFaceInv(p101, p100, p110, p111, R_x, R_y); // Right
        addFaceInv(p011, p111, p110, p010, T_x, T_y); // Top
        addFaceInv(p000, p100, p101, p001, Bm_x, Bm_y); // Bottom
    }

    return cube;
}
