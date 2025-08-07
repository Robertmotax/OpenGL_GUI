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
    shaderUI = new Shader(vertexPathUI, fragmentPathUI);  // Create background
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
    uiElements.push_back({sidebarObj, -1});

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
    transXUp->setOnClick([&](){ if(selectedObject) selectedObject->position.x += 0.05; });
    RenderableObjectStatic* transXDown = new RenderableObjectStatic(tranlateXDown, shaderUI);
    transXDown->setOnClick([&](){ if(selectedObject) selectedObject->position.x -= 0.05; });
    RenderableObjectStatic* transYUp = new RenderableObjectStatic(tranlateYUp, shaderUI);
    transYUp->setOnClick([&](){ if(selectedObject) selectedObject->position.y += 0.05; });
    RenderableObjectStatic* transYDown = new RenderableObjectStatic(tranlateYDown, shaderUI);
    transYDown->setOnClick([&](){ if(selectedObject) selectedObject->position.y -= 0.05; });
    RenderableObjectStatic* transZUp = new RenderableObjectStatic(tranlateZUp, shaderUI);
    transZUp->setOnClick([&](){ if(selectedObject) selectedObject->position.z += 0.05; });
    RenderableObjectStatic* transZDown = new RenderableObjectStatic(tranlateZDown, shaderUI);
    transZDown->setOnClick([&](){ if(selectedObject) selectedObject->position.z -= 0.05; });
    transXUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transYUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transZUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transXDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transYDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    transZDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    uiElements.push_back({transXUp,   0});
    uiElements.push_back({transXDown, 0});
    uiElements.push_back({transYUp   ,0});
    uiElements.push_back({transYDown ,0});
    uiElements.push_back({transZUp   ,0});
    uiElements.push_back({transZDown ,0});

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
    rotXUp->setOnClick([&](){ if(selectedObject) selectedObject->rotation.x += 0.05; });
    RenderableObjectStatic* rotXDown = new RenderableObjectStatic(rotateXDown, shaderUI);
    rotXDown->setOnClick([&](){ if(selectedObject) selectedObject->rotation.x -= 0.05; });
    RenderableObjectStatic* rotYUp = new RenderableObjectStatic(rotateYUp, shaderUI);
    rotYUp->setOnClick([&](){ if(selectedObject) selectedObject->rotation.y += 0.05; });
    RenderableObjectStatic* rotYDown = new RenderableObjectStatic(rotateYDown, shaderUI);
    rotYDown->setOnClick([&](){ if(selectedObject) selectedObject->rotation.y -= 0.05; });
    RenderableObjectStatic* rotZUp = new RenderableObjectStatic(rotateZUp, shaderUI);
    rotZUp->setOnClick([&](){ selectedObject->rotation.z += 0.05; });
    RenderableObjectStatic* rotZDown = new RenderableObjectStatic(rotateZDown, shaderUI);
    rotZDown->setOnClick([&](){ if(selectedObject) selectedObject->rotation.z -= 0.05; });
    rotXUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotYUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotZUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotXDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotYDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    rotZDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    uiElements.push_back({rotXUp   ,0});
    uiElements.push_back({rotXDown ,0});
    uiElements.push_back({rotYUp   ,0});
    uiElements.push_back({rotYDown ,0});
    uiElements.push_back({rotZUp   ,0});
    uiElements.push_back({rotZDown ,0});

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
    sclXUp->setOnClick([&](){ if(selectedObject) selectedObject->scale.x += 0.05; });
    RenderableObjectStatic* sclXDown = new RenderableObjectStatic(scaleXDown, shaderUI);
    sclXDown->setOnClick([&](){ if(selectedObject) selectedObject->scale.x -= 0.05; });
    RenderableObjectStatic* sclYUp = new RenderableObjectStatic(scaleYUp, shaderUI);
    sclYUp->setOnClick([&](){ if(selectedObject) selectedObject->scale.y += 0.05; });
    RenderableObjectStatic* sclYDown = new RenderableObjectStatic(scaleYDown, shaderUI);
    sclYDown->setOnClick([&](){ if(selectedObject) selectedObject->scale.y -= 0.05; });
    RenderableObjectStatic* sclZUp = new RenderableObjectStatic(scaleZUp, shaderUI);
    sclZUp->setOnClick([&](){ if(selectedObject) selectedObject->scale.z += 0.05; });
    RenderableObjectStatic* sclZDown = new RenderableObjectStatic(scaleZDown, shaderUI);
    sclZDown->setOnClick([&](){ if(selectedObject) selectedObject->scale.z -= 0.05; });
    sclXUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclYUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclZUp->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclXDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclYDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    sclZDown->position = glm::vec3(0.0f, 0.0f, 0.1f);
    uiElements.push_back({sclXUp   ,0});
    uiElements.push_back({sclXDown ,0});
    uiElements.push_back({sclYUp   ,0});
    uiElements.push_back({sclYDown ,0});
    uiElements.push_back({sclZUp   ,0});
    uiElements.push_back({sclZDown ,0});

    // Spawn Cube Button
    std::vector<Tri> setParent = createButtonQuad(glm::vec2(-0.95, 0.0f), glm::vec2(0.25, 0.1), glm::vec3(1.0f, 0.4f, 0.4f));
    auto* setParentButton = new RenderableObjectStatic(setParent, shaderUI);
    setParentButton->setName("SetParentButton");
    setParentButton->position = glm::vec3(0.0f, 0.0f, 0.1f);
    setParentButton->setOnClick([]() mutable
    { 
        if(selectedObject)
        {
            waitingForParentSelection = true;
            std::cout << "Choose a scene object to set as the parent for " << selectedObject->getName() << "\n";
        }
    });
    addButton(setParentButton);
    uiElements.push_back({setParentButton, 0});

    //Adding texture selection
    float tileSize = 0.075f;
    int columnCount = 4;
    int index = 0;
    float xInit = -0.975f;
    float yInit = -0.5f;

    for (const auto& entry : fs::directory_iterator("assets/textures")) {
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

        uiElements.push_back({button, 0});
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
    uiElements.push_back({noTextureButton, 0});

    // Create action buttons
    createActionButtons();
}

void Sidebar::createActionButtons() {
    float xPos = -0.95f; float yPos = 0.8f; float width = 0.25f; float height = 0.10f;
    // Spawn Cube Button
    std::vector<Tri> spawnCubeTris = createButtonQuad(glm::vec2(xPos, yPos), glm::vec2(width, height), glm::vec3(0.0f, 0.0f, 1.0f));
    auto* spawnCubeButton = new RenderableObjectStatic(spawnCubeTris, shaderUI);
    spawnCubeButton->setName("CubeButton");
    spawnCubeButton->position = glm::vec3(0.0f, 0.0f, 0.1f);
    spawnCubeButton->setOnClick([]()
    { 
        allObjects.push_back(new RenderableObject(generateCubeTris(0.2f, glm::vec3(0.5f)), defaultShader, shadowShader));
    });
    addButton(spawnCubeButton);
    uiElements.push_back({spawnCubeButton, 1});

    // Spawn Light Button
    yPos -= 0.15;
    std::vector<Tri> spawnLightTris = createButtonQuad(glm::vec2(xPos, yPos), glm::vec2(width, height), glm::vec3(0.8f));
    auto* spawnLightButton = new RenderableObjectStatic(spawnLightTris, shaderUI);
    spawnLightButton->setName("LightButton");
    spawnLightButton->position = glm::vec3(0.0f, 0.0f, 0.1f);
    spawnLightButton->setOnClick([]()
    { 
        LightSource* source = new LightSource(glm::vec3(0.0f), glm::vec3(0.5f), glm::vec3(0.5f), defaultShader, shadowShader);
        source->initShadowCubemap();
        lights.push_back(source);
        RenderableObject* handler = source->lightHandler;
        allObjects.push_back(handler); 
    });
    addButton(spawnLightButton);
    uiElements.push_back({spawnLightButton, 1});

    //spawn another button, this one is set for deletion
    yPos -= 0.15;
    std::vector<Tri> garbageQuad = createButtonQuad(glm::vec2(xPos, yPos), glm::vec2(width, height), glm::vec3(0.0f));
    auto* deleteButton = new RenderableObjectStatic(garbageQuad, shaderUI);
    deleteButton->position = { 0.0f, 0.0f, 0.1f };
    deleteButton->setName("DeleteButton");
    deleteButton->setOnClick([this]() {
        if (!selectedObject) return;

        std::cout << "Deleted: " << selectedObject->getName() << std::endl;
        selectedObject->deleteObject();
        int objectId = selectedObject->id;

        // 1. Delete associated lights first (safe remove & delete)
        auto lit = std::remove_if(lights.begin(), lights.end(),
            [objectId](LightSource* light) {
                return light->lightHandler && light->lightHandler->id == objectId;
            });
        for (auto itr = lit; itr != lights.end(); ++itr) {
            delete *itr;
        }
        lights.erase(lit, lights.end());

        // 2. Remove from allObjects (safe remove & delete)
        auto oit = std::remove_if(allObjects.begin(), allObjects.end(),
            [objectId](RenderableObjectBase* item) {
                auto* obj = dynamic_cast<RenderableObject*>(item);
                return obj && obj->id == objectId;
            });
        for (auto itr = oit; itr != allObjects.end(); ++itr) {
            delete *itr;
        }
        allObjects.erase(oit, allObjects.end());

        // 3. Reset selection (already deleted above)
        selectedObject = nullptr;
    });
    addButton(deleteButton);
    uiElements.push_back({deleteButton, 1});

    //Add model buttons
    int columnCount = 3;
    float tileSize = width/columnCount;
    int index = 0;
    yPos -= 0.075;
    float yPosLoaders = yPos;
    std::vector<glm::vec3> rgb = {{0.1f, 0.0f, 0.0f}, {0.0f, 0.1f, 0.0f}, {0.0f, 0.0f, 0.1f}};

    for (const auto& entry : fs::directory_iterator("assets/models")) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".obj") continue;

        std::string path = entry.path().string();

        int row = index / columnCount;
        int col = index % columnCount;

        float xStart = xPos + col * tileSize;
        float yStart = yPos - row * tileSize;
        glm::vec3 color = (float)row * glm::vec3(0.1f) + rgb[index % columnCount] + glm::vec3(0.4f);

        std::vector<Tri> quad = {
            Tri(
                Vertex{{xStart,yStart, 0.0f}, color, {0.0f, 1.0f}},
                Vertex{{xStart + tileSize, yStart, 0.0f}, color, {1.0f, 1.0f}},
                Vertex{{xStart + tileSize, yStart - tileSize, 0.0f}, color, {1.0f, 0.0f}}
            ),
            Tri(
                Vertex{{xStart, yStart, 0.0f}, color, {0.0f, 1.0f}},
                Vertex{{xStart + tileSize, yStart - tileSize, 0.0f}, color, {1.0f, 0.0f}},
                Vertex{{xStart, yStart - tileSize, 0.0f}, color, {0.0f, 0.0f}}
            )
        };

        auto* button = new RenderableObjectStatic(quad, shaderUI);
        button->position = glm::vec3(0.0f, 0.0f, 0.1f);

        button->setOnClick([path]() {
            OBJLoader objLoader = OBJLoader();
            objLoader.importFromObj(path);
        });

        uiElements.push_back({button, 1});
        ++index;
    }

    //Button to save scene
    yPos = -0.9;
    std::vector<Tri> saveQuad = createButtonQuad(glm::vec2(xPos, yPos), glm::vec2(width, height), glm::vec3(0.0f, 1.0f, 0.0f));
    auto* saveButton = new RenderableObjectStatic(saveQuad, shaderUI);
    saveButton->position = glm::vec3(0.0f, 0.0f, 0.1f);
    saveButton->setName("SaveButton");

    saveButton->setOnClick([this, index, columnCount, xPos, yPosLoaders, tileSize, rgb]() mutable {
        OBJLoader objLoader = OBJLoader();
        std::string filename = objLoader.getAvailableFilename("assets/models", "unnamed", ".obj");
        objLoader.exportToObj(filename);
        std::cout << "Saved Scene " << std::endl;

        int row = index / columnCount;
        int col = index % columnCount;

        float xStart = xPos + col * tileSize;
        float yStart = yPosLoaders - row * tileSize;
        glm::vec3 color = (float)row * glm::vec3(0.1f) + rgb[index % columnCount] + glm::vec3(0.4f);

        std::vector<Tri> quad = {
            Tri(Vertex{{xStart, yStart, 0.0f}, color, {0.0f, 1.0f}},
                Vertex{{xStart + tileSize, yStart, 0.0f}, color, {1.0f, 1.0f}},
                Vertex{{xStart + tileSize, yStart - tileSize, 0.0f}, color, {1.0f, 0.0f}}),
            Tri(Vertex{{xStart, yStart, 0.0f}, color, {0.0f, 1.0f}},
                Vertex{{xStart + tileSize, yStart - tileSize, 0.0f}, color, {1.0f, 0.0f}},
                Vertex{{xStart, yStart - tileSize, 0.0f}, color, {0.0f, 0.0f}})
        };

        auto* button = new RenderableObjectStatic(quad, shaderUI);
        button->position = glm::vec3(0.0f, 0.0f, 0.1f);

        button->setOnClick([filename]() {
            OBJLoader objLoader = OBJLoader();
            objLoader.importFromObj("models/assets/" + filename + ".obj");
        });

        uiElements.push_back({button, 1});
        ++index;
    });

    addButton(saveButton);
    uiElements.push_back({saveButton, 1});
}

std::vector<Tri> Sidebar::createArrow(float x, float y, float offsetY, const glm::vec3& color) {
    return {
        Tri(Vertex{{x, y, 0.1f}, color, {0.0f, 1.0f}},
            Vertex{{x + 0.05f, y, 0.1f}, color, {0.0f, 0.0f}},
            Vertex{{x + 0.025f, y + offsetY, 0.1f}, color, {1.0f, 0.0f}})
    };
}

//modify the visibility of certain ui elements based on the current page
void Sidebar::updateVisibility(GLFWwindow* window) {
    //toggle proper pagination and keyboard handling for Sidebar
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        //handle debounce fast repeating key presses
        if (currentPage < (totalPages - 1)) ++currentPage;
    }
    else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (currentPage > 0) --currentPage;
    }

    for (auto& elem : uiElements) {
        int elemPage = elem.page;
        (elem.object)->setVisible(elemPage == currentPage || elemPage == -1);  // -1 means always visible
    }
}

void Sidebar::setSelectedObject(RenderableObject* obj) {
    selectedObject = obj;
    std::cout << "Selected obj is now " << obj->getName();
}

//Add button to vectors of all buttons available in the scene
void Sidebar::addButton(RenderableObjectStatic* button)  { buttons.push_back(button); }

// Return the specific button to search
RenderableObjectStatic* Sidebar::getButtonByName(const std::string& name) {
    for (auto* elem : buttons) {
        if (elem->getName() == name)
            return dynamic_cast<RenderableObjectStatic*>(elem);
    }
    return nullptr;
}

void Sidebar::render() {
    // Disable depth for UI rendering
    glDisable(GL_DEPTH_TEST);
    for (auto& element : uiElements) {
        auto* obj = element.object;
        if (!obj) continue;
        if (obj->isVisible()) {
            obj->draw(glm::mat4(1.0f), std::vector<LightSource*>());
        }
    }
    glEnable(GL_DEPTH_TEST);
}