// RenderableObjectBase.h
#pragma once

#include <vector>
#include "core/RenderableObjectStatic.h"
#include <core/RenderableObject.h>

class Sidebar {
public:
    Sidebar();
    void render();
    void setSelectedObject(RenderableObject* obj);
    std::vector<RenderableObjectStatic*> uiElements;
private:
    RenderableObject* selectedObject = nullptr;
    const char* vertexPathUI = "shaders/UI.vert";
    const char* fragmentPathUI = "shaders/UI.frag";
    Shader *shaderUI = nullptr;
};