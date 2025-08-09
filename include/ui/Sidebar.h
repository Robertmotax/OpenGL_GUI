// RenderableObjectBase.h
#pragma once

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/RenderableObjectStatic.h"
#include <core/RenderableObject.h>
#include <unordered_map>
#include "core/Texture.h"
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

    //Buttons
    void createTransformButtons(float yPos);
    void createActionButtons(float xPos, float yPos);
    void createAnimationButtons(float xPos, float yPos);

    //update Ui Elements to certain page
    void updateVisibility(GLFWwindow* window);
    void setCurrentPage(int page) { currentPage = page; }
    int getCurrentPage() const { return currentPage; }

    // Utility for creating a UI Button
    SidebarElement* createArrow(float x, float y, int page, std::string name, const glm::vec3& color,  float scale = 1.0f, Texture* texture = nullptr, bool down = false);
    SidebarElement* createButton(float x, float y, int page, std::string name, const glm::vec3& color, float width = SIDEBAR_WIDTH - 2 * PADDING, float height = BUTTON_HEIGHT, Texture* texture = nullptr);
private:
    const char* vertexPathUI = "shaders/UI.vert";
    const char* fragmentPathUI = "shaders/UI.frag";
    Shader *shaderUI = nullptr;

    // For future use if we want multiple pages
    int currentPage = 0;
    static constexpr int TOTAL_PAGES = 3; 
    static constexpr float SIDEBAR_WIDTH = 0.35f;
    static constexpr float BUTTON_HEIGHT = 0.1f;
    static constexpr float PADDING = 0.025f;
    const glm::vec3 SIDEBAR_COLOR = {0.8f, 0.8f, 0.8f};
    const glm::vec3 DEFAULT_COLOR = {0.7f, 0.7f, 0.7f};
    const glm::vec3 HIGHLIGHT_COLOR = {0.7f, 0.7f, 0.2f};

};