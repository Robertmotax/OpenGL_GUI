#pragma once
#include <vector>
#include "core/RenderableObjectBase.h"
#include "core/RenderableObject.h"
#include "core/Camera.h"
#include <GLFW/glfw3.h>

class MouseClickHandler {
public:
    MouseClickHandler(Camera* cam, std::vector<RenderableObjectBase*>* objects)
        : camera(cam), allObjects(objects) {}

    void handleMouseClick(GLFWwindow* window, int button, int action, int mods);
    void handleMouseMove(GLFWwindow* window, double xpos, double ypos);
    void setSelectedDraggableObject(RenderableObject* object);

private:
    Camera* camera;
    std::vector<RenderableObjectBase*>* allObjects;
    //use to define the raycasting
    RenderableObject* selectedDraggableObject = nullptr;
    bool isDragging = false;
};
