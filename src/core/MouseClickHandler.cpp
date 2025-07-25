#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "Camera.h"
#include "RenderableObjectBase.h"

class RayPicker {
public:
    static RayPicker& getInstance();

    RayPicker(const RayPicker&) = delete;
    RayPicker& operator=(const RayPicker&) = delete;
    RayPicker(RayPicker&&) = delete;
    RayPicker& operator=(RayPicker&&) = delete;

    void setCamera(Camera* cam);

    bool screenPosToWorldRay(double mouseX, double mouseY, int screenWidth, int screenHeight,
                             glm::vec3& rayOrigin, glm::vec3& rayDirection) const;

    bool intersectXZPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                          float yLevel, glm::vec3& hitPoint);

    // Only for non-static objects
    RenderableObjectBase* pickObject(double mouseX, double mouseY, int screenWidth, int screenHeight,
                                     const std::vector<RenderableObjectBase*>& objects,
                                     float& outDistance) const;

private:
    RayPicker(); // private constructor
    Camera* camera = nullptr;
};
