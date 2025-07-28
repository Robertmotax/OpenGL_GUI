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

    glm::vec3 vertices[] = {
        {-hs, -hs, -hs}, { hs, -hs, -hs}, { hs,  hs, -hs}, {-hs,  hs, -hs}, // back
        {-hs, -hs,  hs}, { hs, -hs,  hs}, { hs,  hs,  hs}, {-hs,  hs,  hs}  // front
    };

    auto V = [&](int i) { return Vertex{vertices[i], color, {0,0}, {0,0,0}}; };

    // Back face
    tris.emplace_back(V(0), V(1), V(2));
    tris.emplace_back(V(2), V(3), V(0));

    // Front face
    tris.emplace_back(V(4), V(5), V(6));
    tris.emplace_back(V(6), V(7), V(4));

    // Left face
    tris.emplace_back(V(0), V(4), V(7));
    tris.emplace_back(V(7), V(3), V(0));

    // Right face
    tris.emplace_back(V(1), V(5), V(6));
    tris.emplace_back(V(6), V(2), V(1));

    // Top face
    tris.emplace_back(V(3), V(2), V(6));
    tris.emplace_back(V(6), V(7), V(3));

    // Bottom face
    tris.emplace_back(V(0), V(1), V(5));
    tris.emplace_back(V(5), V(4), V(0));

    return tris;
}
