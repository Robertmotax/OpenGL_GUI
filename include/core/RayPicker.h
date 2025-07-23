#pragma once

#include <glm/glm.hpp>
#include "core/Camera.h"

class RayPicker {
public:
    RayPicker(Camera* cam) : camera(cam) {}

    // Convert screen coordinates to world ray
    bool screenPosToWorldRay(double mouseX, double mouseY, int screenWidth, int screenHeight,
                             glm::vec3& rayOrigin, glm::vec3& rayDirection) const;

    // Intersect ray with XZ plane (y = yLevel)
    static bool intersectXZPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                 float yLevel, glm::vec3& hitPoint);

private:
    Camera* camera;
};
