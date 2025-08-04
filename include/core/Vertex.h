#pragma once
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;
    //Designed to handle Element buffer object for models
    bool operator==(const Vertex& other) const {
            return position == other.position && normal == other.normal && texCoord == other.texCoord;
        }
};