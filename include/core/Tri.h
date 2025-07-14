#pragma once

#include "Vertex.h"
#include <glm/glm.hpp>

class Tri {
public:
    Vertex v0, v1, v2;

    Tri(const Vertex& a, const Vertex& b, const Vertex& c) : v0(a), v1(b), v2(c) {}

    bool isClicked(float mouseX, float mouseY) const;
};
