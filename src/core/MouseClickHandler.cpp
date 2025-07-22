#include "core/MouseClickHandler.h"
#include <iostream>

void MouseClickHandler::handleMouseClick(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Get the inverse viewProj matrix from camera
        glm::mat4 invVP = glm::inverse(camera->getViewProjection());

        for (auto& obj : *allObjects) {
            if (obj->isClicked((float)xpos, (float)ypos, width, height, invVP)) {
                if (obj->onClick) {  //verify if onClick is set
                    obj->onClick();
                } else {
                    std::cerr << "Warning: onClick not set for clicked object.\n";
                }
            }
        }
    }
}
