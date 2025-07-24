#include "core/MouseClickHandler.h"
#include "core/RayPicker.h"
#include <iostream>

void MouseClickHandler::handleMouseClick(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Get the inverse viewProj matrix from camera
        glm::mat4 invVP = glm::inverse(camera->getViewProjection());

        glm::vec3 rayOrigin, rayDirection;
        RayPicker::getInstance().screenPosToWorldRay(xpos, ypos, width, height, rayOrigin, rayDirection);


        for (auto& obj : *allObjects) {
            if (obj->isClicked((float)xpos, (float)ypos, width, height, invVP)) {
                obj->onClick();
            }
        }
    }
}

void MouseClickHandler::handleMouseMove(GLFWwindow* window, double xpos, double ypos) {
    if (!isDragging || !selectedObject) return;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glm::vec3 rayOrigin, rayDir;
    RayPicker::getInstance().screenPosToWorldRay(xpos, ypos, width, height, rayOrigin, rayDir);

    glm::vec3 hitPoint;
    if (RayPicker::getInstance().intersectXZPlane(rayOrigin, rayDir, selectedObject->getPosition().y, hitPoint)) {
        selectedObject->setPosition(hitPoint);
    }
}

void MouseClickHandler::setRenderableObject(RenderableObject* object) 
{
    selectedObject = object;
}
