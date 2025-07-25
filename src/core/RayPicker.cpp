#include "core/RayPicker.h"
#include <glm/gtc/matrix_inverse.hpp>

RayPicker::RayPicker() {}

RayPicker& RayPicker::getInstance() {
    static RayPicker instance; // created only once singleton, thread-safe in C++
    return instance;
}

void RayPicker::setCamera(Camera* cam) {
    camera = cam;
}

bool RayPicker::screenPosToWorldRay(double mouseX, double mouseY, int screenWidth, int screenHeight,
                                    glm::vec3& rayOrigin, glm::vec3& rayDirection) const {
    if (!camera) return false;

    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;

    glm::vec4 clipCoords = glm::vec4(x, y, -1.0f, 1.0f);
    glm::mat4 invVP = glm::inverse(camera->getViewProjection());
    glm::vec4 worldPos = invVP * clipCoords;
    worldPos /= worldPos.w;

    rayOrigin = camera->getPosition();
    rayDirection = glm::normalize(glm::vec3(worldPos) - rayOrigin);
    return true;
}

bool RayPicker::intersectXZPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                 float yLevel, glm::vec3& hitPoint) {
    //if the object is lower than current y-plane, which could affect mouse capture                              
    if (fabs(rayDir.y) < 0.0001f) return false;

    float t = (yLevel - rayOrigin.y) / rayDir.y;
    if (t < 0)
        return false;
    //new position    
    hitPoint = rayOrigin + t * rayDir;
    return true;
}
