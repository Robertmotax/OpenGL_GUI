#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "Camera.h"
#include "RenderableObjectBase.h"

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

    bool intersectXZPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                          float yLevel, glm::vec3& hitPoint);

    RenderableObjectBase* RayPicker::pickObject(double mouseX, double mouseY, int screenWidth, int screenHeight,
                    const std::vector<RenderableObjectBase*>& objects, float& outDistance) const
    {
        glm::vec3 rayOrigin, rayDirection;
        if (!screenPosToWorldRay(mouseX, mouseY, screenWidth, screenHeight, rayOrigin, rayDirection)) {
            return nullptr;
        }

        RenderableObjectBase* closest = nullptr;
        float closestDist = FLT_MAX;

        for (auto* obj : objects) {
            glm::vec3 min = obj->getAABBMin(); // You need to implement these functions per object
            glm::vec3 max = obj->getAABBMax();

            float hitDist;
            if (rayIntersectsAABB(rayOrigin, rayDirection, min, max, hitDist)) {
                if (hitDist < closestDist) {
                    closestDist = hitDist;
                    closest = obj;
                }
            }
        }

        outDistance = closestDist;
        return closest;
    }

private:
    RayPicker(); // private constructor
    Camera* camera = nullptr;
};
