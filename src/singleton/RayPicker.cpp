#include "singleton/RayPicker.h"
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
    //Convert mouse coords to Normalized Device Coordinates (NDC)
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight; // Flip-y
    glm::vec4 ndcNear(x, y, -1.0f, 1.0f);
    glm::vec4 ndcFar(x, y, 1.0f, 1.0f);

    //Convert to World Space Ray
    glm::mat4 invVP = glm::inverse(camera->getViewProjection());
    glm::vec4 worldNear = invVP * ndcNear;
    glm::vec4 worldFar = invVP * ndcFar;
    worldNear /= worldNear.w;
    worldFar /= worldFar.w;

    //Return ray origin 
    rayOrigin = glm::vec3(worldNear);
    rayDirection = glm::normalize(glm::vec3(worldFar - worldNear));
    return true;
}

bool RayPicker::intersectPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                               const glm::vec3& planePoint, const glm::vec3& planeNormal, glm::vec3& hitPoint) {
    float denom = glm::dot(rayDir, planeNormal);
    if (fabs(denom) < 1e-6f) return false; // ray is parallel to the plane

    float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
    if (t < 0.0f) return false; // intersection behind the camera

    hitPoint = rayOrigin + t * rayDir;
    return true;
}