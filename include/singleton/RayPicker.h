#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "core/Camera.h"
#include "core/RenderableObjectBase.h"

class RayPicker {
public:
    static RayPicker& getInstance(); // Singleton access

    // Delete copy & move constructors/operators to enforce singleton
    RayPicker(const RayPicker&) = delete;
    RayPicker& operator=(const RayPicker&) = delete;
    RayPicker(RayPicker&&) = delete;
    RayPicker& operator=(RayPicker&&) = delete;

    void setCamera(Camera* cam);
    
    bool screenPosToWorldRay(double mouseX, double mouseY, int screenWidth, int screenHeight,
                             glm::vec3& rayOrigin, glm::vec3& rayDirection) const;

    bool intersectPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& planePoint, 
                        const glm::vec3& planeNormal, glm::vec3& hitPoint);

private:
    RayPicker(); // private constructor
    Camera* camera = nullptr;
};
