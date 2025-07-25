// Sidebar.cpp
#include "ui/Sidebar.h"
#include "core/util.h"
#include "core/Texture.h"
#include "singleton/TextureManager.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <functional>

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

    //load texture
    setTextureTileSelection(uiElements);
}

void Sidebar::setSelectedObject(RenderableObject* obj) {
    selectedObject = obj;
    std::cout << "Selected obj is now " << obj->getName();
}

/** 
 *  @brief Initializes and configures the UI sidebar's texture selection tiles. 
 * The user can choose from a number of static renderable objects (tiles) created by this function.
 * A distinct texture * loaded from the global TextureManager singleton is displayed by each tile.
 * Using custom geometry from * `makeTile()`, Each tile is given a texture and is enabled or disabled appropriately. 
 * The supplied `uiElements` container,
 * which * is subsequently used for rendering and interaction management, receives these tiles after setup.
 * 
 * @param uiElements A vector that contains the sidebar's user interface elements is referenced; 
 */
void Sidebar::setTextureTileSelection(std::vector<RenderableObjectStatic*>& uiElements) 
{
    //All possible tiles for the UI sidebar for texture selection on the object
    std::vector<Tri> tile1 = makeTile(-1.00f, -0.9f, 0.0f, -0.1f, 0.0f, {1.0f, 1.0f, 1.0f});
    std::vector<Tri> tile2 = makeTile(-0.85f, -0.75f, 0.0f, -0.1f, 0.0f, {1.0f, 1.0f, 1.0f});
    std::vector<Tri> tile3 = makeTile(-1.00f, -0.9f, -0.11f, -0.22f, 0.0f, {0.5f, 0.5f, 0.5f});
    std::vector<Tri> grassTile = makeTile(-0.85f, -0.75f, -0.11f, -0.22f, 0.0f, {0.5f, 0.5f, 0.5f});

    //texture renderable static
    RenderableObjectStatic* alaskanTileObj = new RenderableObjectStatic(tile1, shaderUI);
    RenderableObjectStatic* darknessTileObj = new RenderableObjectStatic(tile2, shaderUI);
    RenderableObjectStatic* removeTileObj = new RenderableObjectStatic(tile3, shaderUI);
    RenderableObjectStatic* grassTileObj = new RenderableObjectStatic(grassTile, shaderUI);

    //Set texture enabling and showcasing the texture on screen
    alaskanTileObj->setTexture( TextureManager::getInstance().getTexture("alaskanMalamut"));
    alaskanTileObj->enableTexture(true);
    darknessTileObj->setTexture( TextureManager::getInstance().getTexture("darkness"));
    darknessTileObj->enableTexture(true);
    removeTileObj->setTexture(nullptr); // No texture for the third tile -- assumed this is to remove texture
    removeTileObj->enableTexture(false); 
    grassTileObj->setTexture(TextureManager::getInstance().getTexture("grassland"));
    grassTileObj->enableTexture(true);
    //set the onclick function
    alaskanTileObj->setOnClick([&]() {
        if (selectedObject){
            selectedObject->setTexture(TextureManager::getInstance().getTexture("alaskanMalamut"));
            selectedObject->enableTexture(true);
        }
    });
    darknessTileObj->setOnClick([&]() {
        if (selectedObject){
            selectedObject->setTexture(TextureManager::getInstance().getTexture("darkness"));
            selectedObject->enableTexture(true);
        }
    });
    removeTileObj->setOnClick([&]() {
        if (selectedObject){
            selectedObject->setTexture(nullptr);
            selectedObject->enableTexture(false);
        }
    });
    grassTileObj->setOnClick([&]() {
        if (selectedObject){
            selectedObject->setTexture(TextureManager::getInstance().getTexture("grassland"));
            selectedObject->enableTexture(true);
        }
    });

    uiElements.push_back(alaskanTileObj);
    uiElements.push_back(darknessTileObj);
    uiElements.push_back(removeTileObj);
    uiElements.push_back(grassTileObj); 
}

void Sidebar::render() {
    // Disable depth for UI rendering
    glDisable(GL_DEPTH_TEST);

    for (auto& element : uiElements) {
        element->draw(glm::mat4(1.0f), std::vector<LightSource>());
    }

    glEnable(GL_DEPTH_TEST);
}