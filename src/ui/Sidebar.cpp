// Sidebar.cpp
#include "ui/Sidebar.h"
#include "Globals.h"
#include "core/util.h"
#include "core/Texture.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <functional>
#include <filesystem>
#include "core/OBJLoader.h"
#include <algorithm>
namespace fs = std::filesystem;

Sidebar::~Sidebar() {}

Sidebar::Sidebar()
{
    shaderUI = new Shader(vertexPathUI, fragmentPathUI);

    //Set the background of the side pannel
    std::vector<Tri> sidebar;
    sidebar.emplace_back(
        Vertex{{ -0.65f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}},
        Vertex{{ -0.65f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f}},
        Vertex{{ -1.00f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f}}
    );
    sidebar.emplace_back(
        Vertex{{ -0.65f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f}},
        Vertex{{ -1.00f,  1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 1.0f}},
        Vertex{{ -1.00f, -1.0f, 0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f}}
    );
    RenderableObjectStatic* sidebarObj = new RenderableObjectStatic(sidebar, shaderUI);
    sidebarObj->position = glm::vec3(0.0f);
    uiElements.push_back(sidebarObj);

    //Set clickable objects for translation
    std::vector<Tri> tranlateXUp;
    tranlateXUp.emplace_back(
        Vertex{{ -0.90f ,  0.75f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.95f ,  0.75f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.925f,  0.80f, 0.1f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateXDown;
    tranlateXDown.emplace_back(
        Vertex{{ -0.90f , 0.75f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.95f , 0.75f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.925f, 0.70f, 0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateYUp;
    tranlateYUp.emplace_back(
        Vertex{{ -0.85f ,  0.75f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.80f ,  0.75f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.825f,  0.80f, 0.1f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateYDown;
    tranlateYDown.emplace_back(
        Vertex{{ -0.85f , 0.75f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.80f , 0.75f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.825f, 0.70f, 0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateZUp;
    tranlateZUp.emplace_back(
        Vertex{{ -0.75f , 0.75f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.70f , 0.75f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.725f, 0.80f, 0.1f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> tranlateZDown;
    tranlateZDown.emplace_back(
        Vertex{{ -0.75f , 0.75f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.70f , 0.75f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.725f, 0.70f, 0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    RenderableObjectStatic* transXUp = new RenderableObjectStatic(tranlateXUp, shaderUI);
    transXUp->setOnClick([&](){ selectedObject->position.x += 0.05; });
    RenderableObjectStatic* transXDown = new RenderableObjectStatic(tranlateXDown, shaderUI);
    transXDown->setOnClick([&](){ selectedObject->position.x -= 0.05; });
    RenderableObjectStatic* transYUp = new RenderableObjectStatic(tranlateYUp, shaderUI);
    transYUp->setOnClick([&](){ selectedObject->position.y += 0.05; });
    RenderableObjectStatic* transYDown = new RenderableObjectStatic(tranlateYDown, shaderUI);
    transYDown->setOnClick([&](){ selectedObject->position.y -= 0.05; });
    RenderableObjectStatic* transZUp = new RenderableObjectStatic(tranlateZUp, shaderUI);
    transZUp->setOnClick([&](){ selectedObject->position.z += 0.05; });
    RenderableObjectStatic* transZDown = new RenderableObjectStatic(tranlateZDown, shaderUI);
    transZDown->setOnClick([&](){ selectedObject->position.z -= 0.05; });
    transXUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transYUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transZUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transXDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transYDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transZDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    uiElements.push_back(transXUp);
    uiElements.push_back(transXDown);
    uiElements.push_back(transYUp);
    uiElements.push_back(transYDown);
    uiElements.push_back(transZUp);
    uiElements.push_back(transZDown);

    //Set clickable objects for rotation
    std::vector<Tri> rotateXUp;
    rotateXUp.emplace_back(
        Vertex{{ -0.90f ,  0.50f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.95f ,  0.50f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.925f,  0.55f, 0.1f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> rotateXDown;
    rotateXDown.emplace_back(
        Vertex{{ -0.90f , 0.50f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.95f , 0.50f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.925f, 0.45f, 0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> rotateYUp;
    rotateYUp.emplace_back(
        Vertex{{ -0.85f ,  0.50f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.80f ,  0.50f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.825f,  0.55f, 0.1f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> rotateYDown;
    rotateYDown.emplace_back(
        Vertex{{ -0.85f , 0.50f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.80f , 0.50f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.825f, 0.45f, 0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> rotateZUp;
    rotateZUp.emplace_back(
        Vertex{{ -0.75f , 0.50f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.70f , 0.50f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.725f, 0.55f, 0.1f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> rotateZDown;
    rotateZDown.emplace_back(
        Vertex{{ -0.75f , 0.50f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.70f , 0.50f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.725f, 0.45f, 0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );

    RenderableObjectStatic* rotXUp = new RenderableObjectStatic(rotateXUp, shaderUI);
    rotXUp->setOnClick([&](){ selectedObject->rotation.x += 0.05; });
    RenderableObjectStatic* rotXDown = new RenderableObjectStatic(rotateXDown, shaderUI);
    rotXDown->setOnClick([&](){ selectedObject->rotation.x -= 0.05; });
    RenderableObjectStatic* rotYUp = new RenderableObjectStatic(rotateYUp, shaderUI);
    rotYUp->setOnClick([&](){ selectedObject->rotation.y += 0.05; });
    RenderableObjectStatic* rotYDown = new RenderableObjectStatic(rotateYDown, shaderUI);
    rotYDown->setOnClick([&](){ selectedObject->rotation.y -= 0.05; });
    RenderableObjectStatic* rotZUp = new RenderableObjectStatic(rotateZUp, shaderUI);
    rotZUp->setOnClick([&](){ selectedObject->rotation.z += 0.05; });
    RenderableObjectStatic* rotZDown = new RenderableObjectStatic(rotateZDown, shaderUI);
    rotZDown->setOnClick([&](){ selectedObject->rotation.z -= 0.05; });
    rotXUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotYUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotZUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotXDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotYDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotZDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    uiElements.push_back(rotXUp);
    uiElements.push_back(rotXDown);
    uiElements.push_back(rotYUp);
    uiElements.push_back(rotYDown);
    uiElements.push_back(rotZUp);
    uiElements.push_back(rotZDown);

    //Set 
    std::vector<Tri> scaleXUp;
    scaleXUp.emplace_back(
        Vertex{{ -0.90f ,  0.25f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.95f ,  0.25f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.925f,  0.30f, 0.1f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> scaleXDown;
    scaleXDown.emplace_back(
        Vertex{{ -0.90f , 0.25f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.95f , 0.25f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.925f, 0.20f, 0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> scaleYUp;
    scaleYUp.emplace_back(
        Vertex{{ -0.85f ,  0.25f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.80f ,  0.25f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.825f,  0.30f, 0.1f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> scaleYDown;
    scaleYDown.emplace_back(
        Vertex{{ -0.85f , 0.25f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.80f , 0.25f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.825f, 0.20f, 0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> scaleZUp;
    scaleZUp.emplace_back(
        Vertex{{ -0.75f , 0.25f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.70f , 0.25f, 0.1f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.725f, 0.30f, 0.1f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
    );
    std::vector<Tri> scaleZDown;
    scaleZDown.emplace_back(
        Vertex{{ -0.75f , 0.25f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        Vertex{{ -0.70f , 0.25f, 0.1f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{ -0.725f, 0.20f, 0.1f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
    );
    
    RenderableObjectStatic* sclXUp = new RenderableObjectStatic(scaleXUp, shaderUI);
    sclXUp->setOnClick([&](){ selectedObject->scale.x += 0.05; });
    RenderableObjectStatic* sclXDown = new RenderableObjectStatic(scaleXDown, shaderUI);
    sclXDown->setOnClick([&](){ selectedObject->scale.x -= 0.05; });
    RenderableObjectStatic* sclYUp = new RenderableObjectStatic(scaleYUp, shaderUI);
    sclYUp->setOnClick([&](){ selectedObject->scale.y += 0.05; });
    RenderableObjectStatic* sclYDown = new RenderableObjectStatic(scaleYDown, shaderUI);
    sclYDown->setOnClick([&](){ selectedObject->scale.y -= 0.05; });
    RenderableObjectStatic* sclZUp = new RenderableObjectStatic(scaleZUp, shaderUI);
    sclZUp->setOnClick([&](){ selectedObject->scale.z += 0.05; });
    RenderableObjectStatic* sclZDown = new RenderableObjectStatic(scaleZDown, shaderUI);
    sclZDown->setOnClick([&](){ selectedObject->scale.z -= 0.05; });
    sclXUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclYUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclZUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclXDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclYDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclZDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    uiElements.push_back(sclXUp);
    uiElements.push_back(sclXDown);
    uiElements.push_back(sclYUp);
    uiElements.push_back(sclYDown);
    uiElements.push_back(sclZUp);
    uiElements.push_back(sclZDown);

    //Adding texture selection
    float tileSize = 0.075f;
    int columnCount = 4;
    int index = 0;
    float xInit = -0.975f;
    float yInit = -0.5f;

    for (const auto& entry : fs::directory_iterator("textures")) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".jpg") continue;

        std::string path = entry.path().string();
        Texture* texture = new Texture(path.c_str());

        int row = index / columnCount;
        int col = index % columnCount;

        float xStart = xInit + col * tileSize;
        float yStart = yInit - row * tileSize;

        std::vector<Tri> quad = {
            Tri(
                Vertex{{xStart,yStart, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
                Vertex{{xStart + tileSize, yStart, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
                Vertex{{xStart + tileSize, yStart - tileSize, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}
            ),
            Tri(
                Vertex{{xStart, yStart, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
                Vertex{{xStart + tileSize, yStart - tileSize, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
                Vertex{{xStart, yStart - tileSize, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}
            )
        };

        auto* button = new RenderableObjectStatic(quad, shaderUI);
        button->position = glm::vec3(0.0f, 0.0f, 0.1f);
        button->setTexture(texture);
        button->enableTexture(true);

        button->setOnClick([this, texture, path]() {
            if(selectedObject)
            {
                selectedObject->setTexture(texture);
                selectedObject->enableTexture(true);
                std::cout << "Assigned Texture " << path << " to " << selectedObject->getName() << "\n";
            }
        });

        uiElements.push_back(button);
        ++index;
    }

    int row = index / columnCount;
    int col = index % columnCount;

    float xStart = xInit + col * tileSize;
    float yStart = yInit - row * tileSize;

    std::vector<Tri> grayQuad = {
        Tri(
            Vertex{{xStart, yStart, 0.0f}, {0.6f, 0.6f, 0.6f}, {0.0f, 1.0f}},
            Vertex{{xStart + tileSize, yStart, 0.0f}, {0.6f, 0.6f, 0.6f}, {1.0f, 1.0f}},
            Vertex{{xStart + tileSize, yStart - tileSize, 0.0f}, {0.6f, 0.6f, 0.6f}, {1.0f, 0.0f}}
        ),
        Tri(
            Vertex{{xStart, yStart, 0.0f}, {0.6f, 0.6f, 0.6f}, {0.0f, 1.0f}},
            Vertex{{xStart + tileSize, yStart - tileSize, 0.0f}, {0.6f, 0.6f, 0.6f}, {1.0f, 0.0f}},
            Vertex{{xStart, yStart - tileSize, 0.0f}, {0.6f, 0.6f, 0.6f}, {0.0f, 0.0f}}
        )
    };

    auto* noTextureButton = new RenderableObjectStatic(grayQuad, shaderUI);
    noTextureButton->enableTexture(false); // No texture = clear
    noTextureButton->setOnClick([&]() {
        selectedObject->setTexture(nullptr);
        selectedObject->enableTexture(false);
        std::cout << "Texture removed from " << selectedObject->getName() << "\n";
    });
    uiElements.push_back(noTextureButton);

    // Spawn Cube Button
    std::vector<Tri> spawnCubeTris = createButtonQuad(glm::vec2(-0.95f, -0.9f), glm::vec2(0.2f, 0.08f), glm::vec3(0.2f, 0.6f, 1.0f));
    Button* spawnCubeButton = new Button(spawnCubeTris, shaderUI, "");
    spawnCubeButton->setName("CubeButton");
    spawnCubeButton->position = glm::vec3(-0.8f, -0.3f, 0.1f);
    spawnCubeButton->setOnClick([]()
    { 
        allObjects.push_back(new RenderableObject(makeCube(glm::vec3(1.0f), glm::vec3(0.5f)), defaultShader, shadowShader));
    });

    // Add the newly made Button to UI elements and to vector of buttons
    addButton(spawnCubeButton);
    uiElements.push_back(spawnCubeButton);


    std::vector<Tri> garbageQuad = createButtonQuad(glm::vec2(-0.95f, -0.8f), glm::vec2(0.2f, 0.08f), glm::vec3(0.0f));

    Button* deleteButton = new Button(garbageQuad, shaderUI, "");
    deleteButton->position = glm::vec3(0.0f, 0.0f, 0.1f);
    deleteButton->setName("DeleteButton");

    deleteButton->setOnClick([this]() {
        if (selectedObject) {
            std::cout << "Deleted: " << selectedObject->getName() << std::endl;
            selectedObject->deleteObject();
            auto it = std::find_if(allObjects.begin(), allObjects.end(),
                [](RenderableObjectBase* obj) {
                    return obj == selectedObject; // Access directly
                });

            if (it != allObjects.end()) {
                allObjects.erase(it);
            }
            
            delete selectedObject;
            selectedObject = nullptr;
        }
    });

    addButton(deleteButton);
    uiElements.push_back(deleteButton);

    std::vector<Tri> saveQuad = createButtonQuad(glm::vec2(-0.95, -0.7), glm::vec2(0.2f, 0.08f), glm::vec3(0.2f, 0.6f, 0.2f));
    Button* saveButton = new Button(saveQuad, shaderUI, "");
    saveButton->position = glm::vec3(0.0f, 0.0f, 0.1f);
    saveButton->setName("SaveButton");

    saveButton->setOnClick([]() {
        OBJLoader objLoader = OBJLoader();
        objLoader.exportToObj("models/test.obj");
        std::cout << "Saved Scene " << std::endl;
    });

    addButton(saveButton);
    uiElements.push_back(saveButton);
}

void Sidebar::setSelectedObject(RenderableObject* obj) {
    selectedObject = obj;
    std::cout << "Selected obj is now " << obj->getName();
}

//Add button to vectors of all buttons available in the scene
void Sidebar::addButton(Button* button) 
{
    buttons.push_back(button);
}

// Return the specific button to search
Button* Sidebar::getButtonByName(const std::string& name) {
    for (auto* elem : buttons) {
        if (elem->getName() == name)
            return dynamic_cast<Button*>(elem);
    }
    return nullptr;
}
void Sidebar::render() {
    // Disable depth for UI rendering
    glDisable(GL_DEPTH_TEST);

    for (auto& element : uiElements) {
        element->draw(glm::mat4(1.0f), std::vector<LightSource*>());
    }

    glEnable(GL_DEPTH_TEST);
}