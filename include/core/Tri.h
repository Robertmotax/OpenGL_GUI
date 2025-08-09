#pragma once

#include "Vertex.h"
#include <glm/glm.hpp>

class Tri {
public:
    Vertex v0, v1, v2;

    Tri(const Vertex& a, const Vertex& b, const Vertex& c) : v0(a), v1(b), v2(c) {
        if(v0.normal.s == 0.0f && v0.normal.t == 0.0f && v0.normal.p == 0.0f || v1.normal.s == 0.0f && v1.normal.t == 0.0f && v1.normal.p == 0.0f || v2.normal.s == 0.0f && v2.normal.t == 0.0f && v2.normal.p == 0.0f)
        {
            glm::vec3 edge1 = v1.position - v0.position;
            glm::vec3 edge2 = v2.position - v0.position;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            v0.normal = normal;
            v1.normal = normal;
            v2.normal = normal;
        }
    }
};
