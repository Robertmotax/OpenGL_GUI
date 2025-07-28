// RenderableObjectBase.h
#pragma once

#include <vector>
#include "core/RenderableObjectStatic.h"
#include <core/RenderableObject.h>
#include <unordered_map>
#include "core/Texture.h"
#include "ui/Button.h"
#include "ui/utilSidebar.h"
#include "core/Shader.h"

class Sidebar
{
public:
    Sidebar();
    Sidebar(Shader* shaderShadows);
    ~Sidebar();
    void render();
    void setSelectedObject(RenderableObject* obj);

    //Button
    void addButton(Button* button);


    std::vector<RenderableObjectStatic*> uiElements;
private:
    RenderableObject* selectedObject = nullptr;
    const char* vertexPathUI = "shaders/UI.vert";
    const char* fragmentPathUI = "shaders/UI.frag";
    Shader *shaderUI = nullptr;
    Shader *shaderShadow = nullptr;
    std::vector<Button*> buttons;

};