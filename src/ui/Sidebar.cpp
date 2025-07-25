// Sidebar.cpp
#include "ui/Sidebar.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

Sidebar::Sidebar() 
{
    shaderUI = new Shader(vertexPathUI, fragmentPathUI);
    //Set the background of the side pannel
    std::vector<Tri> sidebar;
    sidebar.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}},
        Vertex{{ -0.6f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f}}
    );
    sidebar.emplace_back(
        Vertex{{ -0.6f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}},
        Vertex{{ -1.0f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 1.0f}},
        Vertex{{ -1.0f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f}}
    );
    RenderableObjectStatic* sidebarObj = new RenderableObjectStatic(sidebar, shaderUI);
    uiElements.push_back(sidebarObj);

    //Set clickable objects for translation
    std::vector<Tri> tranlateXUp;
    tranlateXUp.emplace_back(
        Vertex{{ -0.85f ,  0.85f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.90f ,  0.85f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.875f,  0.90f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateXDown;
    tranlateXDown.emplace_back(
        Vertex{{ -0.85f , 0.85f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.90f , 0.85f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.875f,-0.80f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateYUp;
    tranlateYUp.emplace_back(
        Vertex{{ -0.80f ,  0.85f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.75f ,  0.85f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.775f,  0.90f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateYDown;
    tranlateYDown.emplace_back(
        Vertex{{ -0.80f , 0.85f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.75f , 0.85f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.775f,-0.80f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateZUp;
    tranlateZUp.emplace_back(
        Vertex{{ -0.70f ,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}},
        Vertex{{ -0.65f , -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f}},
        Vertex{{ -0.675f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateZDown;
    tranlateZDown.emplace_back(
        Vertex{{ -0.70f ,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}},
        Vertex{{ -0.65f , -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f}},
        Vertex{{ -0.675f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f}}
    );

}

void Sidebar::setSelectedObject(RenderableObject* obj) {
    selectedObject = obj;
}

void Sidebar::render() {
    // Disable depth for UI rendering
    glDisable(GL_DEPTH_TEST);

    for (auto& element : uiElements) {
        element->draw(glm::mat4(1.0f), std::vector<LightSource>());
    }

    glEnable(GL_DEPTH_TEST);
}