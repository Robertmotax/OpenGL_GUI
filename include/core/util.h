/**
 * @file util.h
 * @brief Utility functions and constants for the project.
 * This file contains helper functions and constants used throughout the project.
 */
#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
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









// Simple ray-AABB test using slab method
bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
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

    tHit = tMin;
    return true;
}
