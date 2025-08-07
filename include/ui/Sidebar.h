// RenderableObjectBase.h
#pragma once

#include <vector>
#include "core/RenderableObjectStatic.h"
#include <core/RenderableObject.h>
#include <unordered_map>
#include "core/Texture.h"
#include "ui/utilSidebar.h"
#include "core/Shader.h"

/**
 * @brief A struct to hold UI elements along with their associated page number.
 * If page is -1, the element is always visible.
 * 
 */
struct SidebarElement {
    RenderableObjectStatic* object;
    int page;  // -1 means always visible
};

class Sidebar
{
public:
    Sidebar();
    ~Sidebar();
    void render();
    void setSelectedObject(RenderableObject* obj);
    //Buttons
    void createActionButtons();
    void addButton(RenderableObjectStatic* button);
    RenderableObjectStatic* getButtonByName(const std::string& name);

    //update Ui Elements to certain page
    void updateVisibility(GLFWwindow* window);
    std::vector<Tri> createArrow(float x, float y, float offsetY, const glm::vec3& color);
    void setCurrentPage(int page) { currentPage = page; }
    int getCurrentPage() const { return currentPage; }

    std::vector<SidebarElement> uiElements;
private:
    const char* vertexPathUI = "shaders/UI.vert";
    const char* fragmentPathUI = "shaders/UI.frag";
    Shader *shaderUI = nullptr;
    std::vector<RenderableObjectStatic*> buttons;

    // For future use if we want multiple pages
    int currentPage = 0;
    const int totalPages = 3; 


};