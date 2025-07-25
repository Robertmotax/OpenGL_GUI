#include "core/MouseClickHandler.h"
#include "core/RayPicker.h"
#include <iostream>

void MouseClickHandler::handleMouseClick(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
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
                    if (obj->onClick) {  //verify if onClick is set
                        obj->onClick();

                        if (obj->isDraggable()) {
                            setSelectedDraggableObject(static_cast<RenderableObject*>(obj));
                            isDragging = true;
                            std::cout << "Object selected and dragging started.\n";
                            return;
                        }
                    } else {
                        std::cerr << "Warning: onClick not set for clicked object.\n";
                    }
                }
            }
        }
        else if (action == GLFW_RELEASE) {
            isDragging = false;
            selectedDraggableObject = nullptr;
            std::cout << "Dragging ended.\n";
        }
    }
}

void MouseClickHandler::handleMouseMove(GLFWwindow* window, double xpos, double ypos) {
    if (!isDragging || !selectedDraggableObject) return;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glm::vec3 rayOrigin, rayDir; // ray that is being "flashed" by the cursor and camera position
    RayPicker::getInstance().screenPosToWorldRay(xpos, ypos, width, height, rayOrigin, rayDir);

    glm::vec3 hitPoint;
    if (RayPicker::getInstance().intersectXZPlane(rayOrigin, rayDir, selectedDraggableObject->getPosition().y, hitPoint)) {
        selectedDraggableObject->setPosition(hitPoint);
    }
}

void MouseClickHandler::setSelectedDraggableObject(RenderableObject* object) 
{
    selectedDraggableObject = object;
}
