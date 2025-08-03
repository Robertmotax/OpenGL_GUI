/**
 * @file utilSidebar.h
 * @brief Utility functions mostly designed for the sidebar .
 *
 */
#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <GL/glew.h>
#include "core/Vertex.h"
#include "core/Tri.h"
#include "core/Shader.h"
#include "core/Texture.h"
#include "core/LightSource.h"
#include "core/RenderableObject.h"
#include <iostream>
#include <vector>


/**
 * @brief create a button for the sidebar
 */
inline std::vector<Tri> createButtonQuad(glm::vec2 position, glm::vec2 size, glm::vec3 color)
{
    float x = position.x, y = position.y;
    float w = size.x, h = size.y;

    glm::vec3 col = color;

    std::vector<Tri> tris;
    tris.emplace_back(
        Vertex{{x,     y,     0}, col, {0, 0}, {0, 0, 0}},
        Vertex{{x + w, y,     0}, col, {1, 0}, {0, 0, 0}},
        Vertex{{x + w, y + h, 0}, col, {1, 1}, {0, 0, 0}}
    );
    tris.emplace_back(
        Vertex{{x,     y,     0}, col, {0, 0}, {0, 0, 0}},
        Vertex{{x + w, y + h, 0}, col, {1, 1}, {0, 0, 0}},
        Vertex{{x,     y + h, 0}, col, {0, 1}, {0, 0, 0}}
    );
    return tris;
}


/**
 * generate the cube texture that we can add texture
 */
inline std::vector<Tri> generateCubeTris(float size = 1.0f, glm::vec3 color = {0.8f, 0.8f, 0.8f})
{
    std::vector<Tri> tris;
    float hs = size * 0.5f;

    glm::vec3 v[] = {
        {-hs, -hs, -hs}, { hs, -hs, -hs}, { hs,  hs, -hs}, {-hs,  hs, -hs}, // back  0–3
        {-hs, -hs,  hs}, { hs, -hs,  hs}, { hs,  hs,  hs}, {-hs,  hs,  hs}  // front 4–7
    };

    glm::vec2 uv01 = {0.0f, 1.0f};
    glm::vec2 uv00 = {0.0f, 0.0f};
    glm::vec2 uv10 = {1.0f, 0.0f};
    glm::vec2 uv11 = {1.0f, 1.0f};

    auto V = [&](glm::vec3 pos, glm::vec2 uv) {
        return Vertex{pos, color, uv, glm::vec3(0.0f)};
    };

    // Back face (0, 1, 2, 3)
    tris.emplace_back(V(v[1], uv00), V(v[0], uv01), V(v[2], uv10));
    tris.emplace_back(V(v[3], uv00), V(v[2], uv01), V(v[0], uv10));

    // Front face (4, 5, 6, 7)
    tris.emplace_back(V(v[4], uv01), V(v[5], uv00), V(v[6], uv10));
    tris.emplace_back(V(v[6], uv01), V(v[7], uv00), V(v[4], uv10));

    // Left face (0, 4, 7, 3)
    tris.emplace_back(V(v[0], uv01), V(v[4], uv00), V(v[7], uv10));
    tris.emplace_back(V(v[7], uv01), V(v[3], uv00), V(v[0], uv10));

    // Right face (1, 5, 6, 2)
    tris.emplace_back(V(v[5], uv00), V(v[1], uv01), V(v[6], uv10));
    tris.emplace_back(V(v[2], uv00), V(v[6], uv01), V(v[1], uv10));

    // Top face (3, 2, 6, 7)
    tris.emplace_back(V(v[2], uv00), V(v[3], uv01), V(v[6], uv10));
    tris.emplace_back(V(v[7], uv00), V(v[6], uv01), V(v[3], uv10));

    // Bottom face (0, 1, 5, 4)
    tris.emplace_back(V(v[0], uv01), V(v[1], uv00), V(v[5], uv10));
    tris.emplace_back(V(v[5], uv01), V(v[4], uv00), V(v[0], uv10));

    return tris;
}
