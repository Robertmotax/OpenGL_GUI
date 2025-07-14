#pragma once
#include <vector>
#include "core/RenderableObjectBase.h"
#include "core/Camera.h"
#include <GLFW/glfw3.h>

class MouseClickHandler {
public:
    MouseClickHandler(Camera* cam, std::vector<RenderableObjectBase*>* objects)
        : camera(cam), allObjects(objects) {}

    void handleMouseClick(GLFWwindow* window, int button, int action, int mods);

private:
    Camera* camera;
    std::vector<RenderableObjectBase*>* allObjects;
};
