#define GLM_ENABLE_EXPERIMENTAL
#include "core/Tri.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

bool Tri::isClicked(float x, float y) const {
    glm::vec2 p(x, y);
    glm::vec2 a(v0.position.x, v0.position.y);
    glm::vec2 b(v1.position.x, v1.position.y);
    glm::vec2 c(v2.position.x, v2.position.y);

    glm::vec2 v0 = b - a;
    glm::vec2 v1 = c - a;
    glm::vec2 v2 = p - a;

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;
    if (denom == 0.0f) return false; // Degenerate triangle

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return (u >= 0) && (v >= 0) && (w >= 0);
}

