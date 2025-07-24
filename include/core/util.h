/**
 * @file util.h
 * @brief Utility functions and constants for the project.
 * This file contains helper functions and constants used throughout the project.
 */
#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include "core/Vertex.h"
#include "core/Tri.h"
#include <vector>

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