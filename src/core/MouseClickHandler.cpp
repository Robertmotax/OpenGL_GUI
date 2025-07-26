#include "core/MouseClickHandler.h"
#include <iostream>
#include <singleton/RayPicker.h>
#include <core/RenderableObject.h>
#include <core/RenderableObjectStatic.h>

void MouseClickHandler::handleMouseClick(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            glm::mat4 invVP = glm::inverse(camera->getViewProjection());

            RenderableObjectBase* closestObject = nullptr;
            float closestDistance = FLT_MAX;

            bool uiClick = false;
            for (auto& obj : *allObjects) {
                float dist;
                if (obj->isClicked((float)xpos, (float)ypos, width, height, invVP, dist)) {
                    //Mainly to avoid static objects being affected
                    if(auto* staticObj = dynamic_cast<RenderableObjectStatic*>(obj))
                    {
                        if(closestObject == nullptr || closestObject->position.z <= staticObj->position.z){
                            closestObject = staticObj;
                            uiClick = true;
                        }
                    }

                    //Mainly for non-static 
                    if (dist < closestDistance && !uiClick) {
                        closestDistance = dist;
                        if(auto* sceneObj = dynamic_cast<RenderableObject*>(obj)) {
                            closestObject = sceneObj;
                            setSelectedDraggableObject(sceneObj);
                            isDragging = true;
                        }
                    }
                }
            }

            if (closestObject && closestObject->onClick) {
                std::cout << "Clicked on " << closestObject->getName() << std::endl;
                closestObject->onClick();
            } else if (closestObject) {
                std::cerr << "Warning: onClick not set for clicked object.\n";
            }
        }
        else if (action == GLFW_RELEASE) {
            //manually reset it to false and nullptr, even if non-static is selected
            isDragging = false;
            setSelectedDraggableObject(nullptr);
        }
    }
}

void MouseClickHandler::handleMouseMove(GLFWwindow* window, double xpos, double ypos) {
    if (!isDragging || !selectedDraggableObject) return;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glm::vec3 rayOrigin, rayDir;
    RayPicker::getInstance().screenPosToWorldRay(xpos, ypos, width, height, rayOrigin, rayDir);

    glm::vec3 hitPoint;
    glm::vec3 objectPos = selectedDraggableObject->getPosition();
    glm::vec3 cameraForward = glm::normalize(camera->getCameraFront() - camera->getPosition()); // Assuming you have camera access

    if (RayPicker::getInstance().intersectPlane(rayOrigin, rayDir, objectPos, cameraForward, hitPoint)) {
        selectedDraggableObject->setPosition(hitPoint);
    }
}


void MouseClickHandler::setSelectedDraggableObject(RenderableObject* object) 
{
    selectedDraggableObject = object;
}
