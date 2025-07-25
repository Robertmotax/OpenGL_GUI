#include "core/MouseClickHandler.h"
#include "core/RayPicker.h"
#include <iostream>
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

            glm::vec3 rayOrigin, rayDirection;
            RayPicker::getInstance().screenPosToWorldRay(xpos, ypos, width, height, rayOrigin, rayDirection);

        RenderableObjectBase* closestObject = nullptr;
        float closestDistance = FLT_MAX;

        bool uiClick = false;
        for (auto& obj : *allObjects) {
            float dist;
            if (obj->isClicked((float)xpos, (float)ypos, width, height, invVP, dist)) {
                if(auto* staticObj = dynamic_cast<RenderableObjectStatic*>(obj))
                {
                    if(closestObject == nullptr || closestObject->position.z <= staticObj->position.z){
                        closestObject = staticObj;
                        uiClick = true;
                    }
                }
                if (dist < closestDistance && !uiClick) {
                    closestDistance = dist;
                    if(auto* sceneObj = dynamic_cast<RenderableObject*>(obj))
                        closestObject = sceneObj;
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
}

