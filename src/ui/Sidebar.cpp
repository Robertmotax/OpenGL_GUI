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
        Vertex{{ -1.00f + SIDEBAR_WIDTH,  1.0f, 0.0f}, SIDEBAR_COLOR, {0.0f, 1.0f}},
        Vertex{{ -1.00f + SIDEBAR_WIDTH, -1.0f, 0.0f}, SIDEBAR_COLOR, {0.0f, 0.0f}},
        Vertex{{ -1.00f, -1.0f, 0.0f}, SIDEBAR_COLOR, {1.0f, 0.0f}}
    );
    sidebar.emplace_back(
        Vertex{{ -1.00f + SIDEBAR_WIDTH,  1.0f, 0.0f}, SIDEBAR_COLOR, {0.0f, 1.0f}},
        Vertex{{ -1.00f,  1.0f, 0.0f}, SIDEBAR_COLOR, {1.0f, 1.0f}},
        Vertex{{ -1.00f, -1.0f, 0.0f}, SIDEBAR_COLOR, {1.0f, 0.0f}}
    );
    RenderableObjectStatic* sidebarObj = new RenderableObjectStatic(sidebar, shaderUI);
    sidebarObj->position = glm::vec3(0.0f);
    uiElements.push_back(new SidebarElement{sidebarObj, -1});

    float xStart = -1.0f + PADDING;
    float yStart = 1.0f - PADDING;

    Texture *prevButtonTexture = new Texture("assets\\textures\\sidebar\\PrevButton.jpg");
    SidebarElement *prevButton = createButton(-1.0f, 1.0f, -1, "PrevNextIndicator", {1.0f, 0.0f, 0.0f}, 0.05f, 0.033f, prevButtonTexture);
    prevButton->object->setOnClick([this]() mutable
    {
        if(currentPage > 0) 
            currentPage--;
        for (auto* elem : uiElements) {
            int elemPage = elem->page;
            elem->object->setVisible(elemPage == currentPage || elemPage == -1);
        }
    });

    Texture *nextButtonTexture = new Texture("assets\\textures\\sidebar\\NextButton.jpg");
    SidebarElement *nextButton = createButton(-0.95f, 1.0f, -1, "PrevNextIndicator", {1.0f, 0.0f, 0.0f}, 0.05f, 0.033f, nextButtonTexture);
    nextButton->object->setOnClick([this]() mutable
    {
        if(currentPage < (Sidebar::TOTAL_PAGES-1)) 
            currentPage++; 
        for (auto* elem : uiElements) {
            int elemPage = elem->page;
            elem->object->setVisible(elemPage == currentPage || elemPage == -1);
        }
    });

    // Create transformation buttons
    createTransformButtons(yStart);

    // Create texture selector buttons
    createActionButtons(xStart, yStart);

    //Create animation buttons
    createAnimationButtons(xStart, yStart);
}

void Sidebar::createTransformButtons(float yStart) {
    glm::vec3 green = {0.0f, 1.0f, 0.0f};
    glm::vec3 red = {1.0f, 0.0f, 0.0f};
    float smallScale = 0.7f;
    float yOffset = 0.05f;
    float xOffset = (0.05f / 2) - (0.05f / 2) * smallScale;

    // Set clickable objects for translation
    float xStart = -1.0 + PADDING;
    yStart -= 0.15f;

    // Original translation arrows
    SidebarElement *transXUp = createArrow(xStart, yStart, 0, "translateXUp", green);
    SidebarElement *transXDown = createArrow(xStart, yStart, 0, "translateXDown", red, 1.0f, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *transYUp = createArrow(xStart, yStart, 0, "translateYUp", green);
    SidebarElement *transYDown = createArrow(xStart, yStart, 0, "translateYDown", red, 1.0f, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *transZUp = createArrow(xStart, yStart, 0, "translateZUp", green);
    SidebarElement *transZDown = createArrow(xStart, yStart, 0, "translateZDown", red, 1.0f, nullptr, true);

    // Fast translation arrows
    float fastStep = 0.25f;

    SidebarElement *transXUpFast = createArrow(xStart - 2 * ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart + yOffset, 0, "translateXUpFast", green, smallScale, nullptr, false);
    SidebarElement *transXDownFast = createArrow(xStart - 2 * ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart - yOffset, 0, "translateXDownFast", red, smallScale, nullptr, true);
    SidebarElement *transYUpFast = createArrow(xStart - ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart + yOffset, 0, "translateYUpFast", green, smallScale, nullptr, false);
    SidebarElement *transYDownFast = createArrow(xStart - ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart - yOffset, 0, "translateYDownFast", red, smallScale, nullptr, true);
    SidebarElement *transZUpFast = createArrow(xStart + xOffset, yStart + yOffset, 0, "translateZUpFast", green, smallScale, nullptr, false);
    SidebarElement *transZDownFast = createArrow(xStart + xOffset, yStart - yOffset, 0, "translateZDownFast", red, smallScale, nullptr, true);

    // Click handlers for translation
    transXUp->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->position.x += 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transXDown->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->position.x -= 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transYUp->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->position.y += 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transYDown->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->position.y -= 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transZUp->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->position.z += 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transZDown->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->position.z -= 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });

    // Fast movement handlers
    transXUpFast->object->setOnClick([fastStep]() {
        if (selectedObject) {
            selectedObject->position.x += fastStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transXDownFast->object->setOnClick([fastStep]() {
        if (selectedObject) {
            selectedObject->position.x -= fastStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transYUpFast->object->setOnClick([fastStep]() {
        if (selectedObject) {
            selectedObject->position.y += fastStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transYDownFast->object->setOnClick([fastStep]() {
        if (selectedObject) {
            selectedObject->position.y -= fastStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transZUpFast->object->setOnClick([fastStep]() {
        if (selectedObject) {
            selectedObject->position.z += fastStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    transZDownFast->object->setOnClick([fastStep]() {
        if (selectedObject) {
            selectedObject->position.z -= fastStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });

    xStart = -1.0 + PADDING;
    yStart -= 0.2f;

    // Original rotation arrows
    SidebarElement *rotXUp = createArrow(xStart, yStart, 0, "rotateXUp", green);
    SidebarElement *rotXDown = createArrow(xStart, yStart, 0, "rotateXDown", red, 1.0f, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *rotYUp = createArrow(xStart, yStart, 0, "rotateYUp", green);
    SidebarElement *rotYDown = createArrow(xStart, yStart, 0, "rotateYDown", red, 1.0f, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *rotZUp = createArrow(xStart, yStart, 0, "rotateZUp", green);
    SidebarElement *rotZDown = createArrow(xStart, yStart, 0, "rotateZDown", red, 1.0f, nullptr, true);

    // Fast rotation arrows
    float fastRotStep = 0.25f;

    SidebarElement *rotXUpFast = createArrow(xStart - 2 * ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart + yOffset, 0, "rotateXUpFast", green, smallScale, nullptr, false);
    SidebarElement *rotXDownFast = createArrow(xStart - 2 * ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart - yOffset, 0, "rotateXDownFast", red, smallScale, nullptr, true);
    SidebarElement *rotYUpFast = createArrow(xStart - ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart + yOffset, 0, "rotateYUpFast", green, smallScale, nullptr, false);
    SidebarElement *rotYDownFast = createArrow(xStart - ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart - yOffset, 0, "rotateYDownFast", red, smallScale, nullptr, true);
    SidebarElement *rotZUpFast = createArrow(xStart + xOffset, yStart + yOffset, 0, "rotateZUpFast", green, smallScale, nullptr, false);
    SidebarElement *rotZDownFast = createArrow(xStart + xOffset, yStart - yOffset, 0, "rotateZDownFast", red, smallScale, nullptr, true);

    // Rotation handlers
    rotXUp->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->rotation.x += 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotXDown->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->rotation.x -= 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotYUp->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->rotation.y += 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotYDown->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->rotation.y -= 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotZUp->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->rotation.z += 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotZDown->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->rotation.z -= 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });

    // Fast rotation handlers
    rotXUpFast->object->setOnClick([fastRotStep]() {
        if (selectedObject) {
            selectedObject->rotation.x += fastRotStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotXDownFast->object->setOnClick([fastRotStep]() {
        if (selectedObject) {
            selectedObject->rotation.x -= fastRotStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotYUpFast->object->setOnClick([fastRotStep]() {
        if (selectedObject) {
            selectedObject->rotation.y += fastRotStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotYDownFast->object->setOnClick([fastRotStep]() {
        if (selectedObject) {
            selectedObject->rotation.y -= fastRotStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotZUpFast->object->setOnClick([fastRotStep]() {
        if (selectedObject) {
            selectedObject->rotation.z += fastRotStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    rotZDownFast->object->setOnClick([fastRotStep]() {
        if (selectedObject) {
            selectedObject->rotation.z -= fastRotStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });

    xStart = -1.0 + PADDING;
    yStart -= 0.2f;

    // Original scale arrows
    SidebarElement *scaleXUp = createArrow(xStart, yStart, 0, "scaleXUp", green);
    SidebarElement *scaleXDown = createArrow(xStart, yStart, 0, "scaleXDown", red, 1.0f, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *scaleYUp = createArrow(xStart, yStart, 0, "scaleYUp", green);
    SidebarElement *scaleYDown = createArrow(xStart, yStart, 0, "scaleYDown", red, 1.0f, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *scaleZUp = createArrow(xStart, yStart, 0, "scaleZUp", green);
    SidebarElement *scaleZDown = createArrow(xStart, yStart, 0, "scaleZDown", red, 1.0f, nullptr, true);

    // Fast scale arrows
    float fastScaleStep = 0.25f;

    SidebarElement *scaleXUpFast = createArrow(xStart - 2 * ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart + yOffset, 0, "scaleXUpFast", green, smallScale, nullptr, false);
    SidebarElement *scaleXDownFast = createArrow(xStart - 2 * ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart - yOffset, 0, "scaleXDownFast", red, smallScale, nullptr, true);
    SidebarElement *scaleYUpFast = createArrow(xStart - ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart + yOffset, 0, "scaleYUpFast", green, smallScale, nullptr, false);
    SidebarElement *scaleYDownFast = createArrow(xStart - ((SIDEBAR_WIDTH - 2 * PADDING) / 3) + xOffset, yStart - yOffset, 0, "scaleYDownFast", red, smallScale, nullptr, true);
    SidebarElement *scaleZUpFast = createArrow(xStart + xOffset, yStart + yOffset, 0, "scaleZUpFast", green, smallScale, nullptr, false);
    SidebarElement *scaleZDownFast = createArrow(xStart + xOffset, yStart - yOffset, 0, "scaleZDownFast", red, smallScale, nullptr, true);

    // Scale handlers
    scaleXUp->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->scale.x += 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleXDown->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->scale.x = std::max(0.05f, selectedObject->scale.y - 0.05f);
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleYUp->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->scale.y += 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleYDown->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->scale.y = std::max(0.05f, selectedObject->scale.y - 0.05f);
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleZUp->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->scale.z += 0.05f;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleZDown->object->setOnClick([&]() {
        if (selectedObject) {
            selectedObject->scale.z = std::max(0.05f, selectedObject->scale.y - 0.05f);
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });

    // Fast scale handlers
    scaleXUpFast->object->setOnClick([fastScaleStep]() {
        if (selectedObject) {
            selectedObject->scale.x += fastScaleStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleXDownFast->object->setOnClick([fastScaleStep]() {
        if (selectedObject) {
            selectedObject->scale.x = std::max(0.05f, selectedObject->scale.x - fastScaleStep);
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleYUpFast->object->setOnClick([fastScaleStep]() {
        if (selectedObject) {
            selectedObject->scale.y += fastScaleStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleYDownFast->object->setOnClick([fastScaleStep]() {
        if (selectedObject) {
            selectedObject->scale.y = std::max(0.05f, selectedObject->scale.y - fastScaleStep);
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleZUpFast->object->setOnClick([fastScaleStep]() {
        if (selectedObject) {
            selectedObject->scale.z += fastScaleStep;
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    scaleZDownFast->object->setOnClick([fastScaleStep]() {
        if (selectedObject) {
            selectedObject->scale.z = std::max(0.05f, selectedObject->scale.z - fastScaleStep);
            selectedObject->updateLocalTransformFromComponents();
            selectedObject->updateSelfAndChildren();
        }
    });
    // Set hierarchy Button
    yStart -= 0.15f;
    xStart = -1.0f + PADDING;
    Texture *setParentButtonTexture = new Texture("assets\\textures\\sidebar\\SetParentButton.jpg");
    SidebarElement *hierarchyButton = createButton(xStart, yStart, 0, "SetHierarchyButton", {1.0f, 0.0f, 0.0f}, 0.3f, 0.1f, setParentButtonTexture);
    hierarchyButton->object->setOnClick([]() mutable
    { 
        if(selectedObject)
        {
            waitingForParentSelection = true;
            std::cout << "Choose a scene object to set as the parent for " << selectedObject->getName() << "\n";
        }
    });

    // Remove hierarchy Button
    yStart -= 0.15f;
    Texture *removeHierarchyButtonTexture = new Texture("assets\\textures\\sidebar\\RemoveParentButton.jpg");
    SidebarElement *removeHierarchyButton = createButton(xStart, yStart, 0, "RemoveHierarchyButton", {0.0f, 0.0f, 0.0f}, 0.3f, 0.1f, removeHierarchyButtonTexture);
    removeHierarchyButton->object->setOnClick([]()
    { 
        if(selectedObject && selectedObject->parent)
        {
            selectedObject->setParent(nullptr);
            std::cout << "Removed parent from " << selectedObject->getName() << "\n";
        }
    });

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
        Texture* texture = new Texture(path);

        int row = index / columnCount;
        int col = index % columnCount;

        float xStart = xInit + col * tileSize;
        float yStart = yInit - row * tileSize;

        SidebarElement *textButton = createButton(xStart, yStart, 0, "TextureButton_" + path, glm::vec3(0.0f), tileSize, tileSize, texture);

        textButton->object->setOnClick([this, texture, path]() {
            if(selectedObject)
            {
                selectedObject->setTexture(texture);
                selectedObject->enableTexture(true);
                std::cout << "Assigned Texture " << path << " to " << selectedObject->getName() << "\n";
            }
        });
        ++index;
    }

    int row = index / columnCount;
    int col = index % columnCount;

    xStart = xInit + col * tileSize;
    yStart = yInit - row * tileSize;

    SidebarElement *textButton = createButton(xStart, yStart, 0, "NoTextureButton", glm::vec3(0.5f), tileSize, tileSize);
    textButton->object->setOnClick([&]() {
        selectedObject->setTexture(nullptr);
        selectedObject->enableTexture(false);
        std::cout << "Texture removed from " << selectedObject->getName() << "\n";
    });
}


void Sidebar::createActionButtons(float xPos, float yPos) {
    // Spawn Cube Button
    yPos -= 0.025;
    Texture *spawnCubeButtonTexture = new Texture("assets\\textures\\sidebar\\AddCubeButton.jpg");
    SidebarElement* spawnCubeButton = createButton(xPos, yPos, 1, "SpawnCubeButton", glm::vec3(0.0f, 0.0f, 1.0f), 0.3f, 0.1f, spawnCubeButtonTexture);
    spawnCubeButton->object->setOnClick([]()
    {
        RenderableObject *cube = new RenderableObject(makeCube(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.5f)), defaultShader, shadowShader);
        selectedObject = cube;
        allObjects.push_back(cube); 
    });

    // Spawn Light Button
    yPos -= 0.15;
    Texture *spawnLightButtonTexture = new Texture("assets\\textures\\sidebar\\AddLightButton.jpg");
    SidebarElement* spawnLightButton = createButton(xPos, yPos, 1, "SpawnLightButton", glm::vec3(0.9f), 0.3f, 0.1f, spawnLightButtonTexture);
    spawnLightButton->object->setOnClick([]()
    { 
        LightSource* source = new LightSource(glm::vec3(0.0f), glm::vec3(0.5f), glm::vec3(0.8f), defaultShader, shadowShader);
        source->initShadowCubemap();
        lights.push_back(source);
        RenderableObject* handler = source->lightHandler;
        allObjects.push_back(handler); 
    });

    //spawn another button, this one is set for deletion
    yPos -= 0.15;
    Texture *deleteObjectButtonTexture = new Texture("assets\\textures\\sidebar\\DeleteObjectButton.jpg");
    SidebarElement* deleteButton = createButton(xPos, yPos, 1, "DeleteObjectButton", glm::vec3(0.0f), 0.3f, 0.1f, deleteObjectButtonTexture);
    deleteButton->object->setOnClick([this]() 
    {
        if (!selectedObject) return;

        std::cout << "Deleted: " << selectedObject->getName() << std::endl;

        int objectId = selectedObject->id;

        // 1. Remove and delete lights safely
        auto lit = std::remove_if(lights.begin(), lights.end(),
            [objectId](LightSource* light) {
                return light->lightHandler && light->lightHandler->id == objectId;
            });

        for (auto itr = lit; itr != lights.end(); ++itr) {
            delete *itr;
        }
        lights.erase(lit, lights.end());

        // 2. Delete the object once, then remove from allObjects
        // (Make a copy of the pointer to delete it before erasing from the vector)
        RenderableObjectBase* toDelete = selectedObject;
        selectedObject = nullptr;

        allObjects.erase(
            std::remove_if(allObjects.begin(), allObjects.end(),
                [toDelete](RenderableObjectBase* item) {
                    return item == toDelete;
                }),
            allObjects.end()
        );

        delete toDelete;
    });

    //Add model buttons
    int columnCount = 3;
    float tileSize = (SIDEBAR_WIDTH - 2 * PADDING)/columnCount;
    int index = 0;
    yPos -= 0.15;
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

        SidebarElement* loaderButton = createButton(xStart, yStart, 1, "loader" + std::to_string(index), color, tileSize, tileSize);

        loaderButton->object->setOnClick([path]() {
            OBJLoader objLoader = OBJLoader();
            objLoader.load(path);
            lastTime = -1.0f;
        });
        keyframeButtons.push_back(loaderButton->object);
        ++index;
    }

    //Button to save scene
    yPos = -0.875;
    Texture *saveButtonTexture = new Texture("assets\\textures\\sidebar\\SaveButton.jpg");
    SidebarElement* saveButton = createButton(xPos, yPos, 1, "SaveSceneButton", glm::vec3(0.0f, 1.0f, 0.0f), 0.3f, 0.1f, saveButtonTexture);

    saveButton->object->setOnClick([this, index, columnCount, xPos, yPosLoaders, tileSize, rgb]() mutable {
        OBJLoader objLoader = OBJLoader();
        std::string filename = objLoader.getAvailableFilename("assets/models", "unnamed", ".obj");
        objLoader.save(filename);
        std::cout << "Saved Scene " << std::endl;

        int row = index / columnCount;
        int col = index % columnCount;

        float xStart = xPos + col * tileSize;
        float yStart = yPosLoaders - row * tileSize;
        glm::vec3 color = (float)row * glm::vec3(0.1f) + rgb[index % columnCount] + glm::vec3(0.4f);

        SidebarElement* loaderButton = createButton(xStart, yStart, 1, "loader" + std::to_string(index), color, tileSize, tileSize);

        loaderButton->object->setOnClick([filename]() {
            OBJLoader objLoader = OBJLoader();
            objLoader.load(filename);
            lastTime = -1.0f;
        });
        allObjects.push_back(loaderButton->object);

        ++index;
    });
}

void Sidebar::createAnimationButtons(float xPos, float yPos)
{
    // Add animation buttons
    yPos -= 0.025;
    float secondButtonSize = 0.02f;
    int numberOfSeconds = 60;
    int timeIndex = 0;

    while (timeIndex < numberOfSeconds) {
        int currentTime = timeIndex;
        float xStart = xPos + (currentTime % ((int)(0.3f / secondButtonSize))) * secondButtonSize;
        float yStart = yPos - (secondButtonSize + 0.02f) * (currentTime / (int)(0.3f / secondButtonSize));

        SidebarElement *timeButton;
        glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);
        if(timeIndex != 0)
        {
            timeButton = createButton(xStart, yStart, 2, "TimeButton" + std::to_string(currentTime), color, secondButtonSize, secondButtonSize);
        }
        else
        {
            Texture* texture = new Texture("assets/textures/sidebar/SelectedElementColor.jpg", true);
            timeButton = createButton(xStart, yStart, 2, "TimeButton" + std::to_string(currentTime), color, secondButtonSize, secondButtonSize, texture);
        }

        timeButton->object->setOnClick([timeButton, currentTime]() {
            for(auto* elm : keyframeButtons)
            {
                if(elm->getName() == "TimeButton" + std::to_string((int)sceneTime))
                {
                    bool keyed = false;
                    if(selectedObject)
                    {
                        for(Keyframe& key : selectedObject->keyframes)
                        {
                            if((int)key.time == (int)sceneTime)
                            {
                                keyed = true;
                            }
                        }
                    }
                    Texture *keyframePresentColor = new Texture("assets/textures/sidebar/KeyframedElementColor.jpg");
                    if(keyed)
                    {
                        elm->setTexture(keyframePresentColor);
                        elm->enableTexture(true);
                    }
                    else
                    {
                        elm->setTexture(nullptr);
                        elm->enableTexture(false);
                    }
                    break;
                }
            }
            Texture* texture = new Texture("assets/textures/sidebar/SelectedElementColor.jpg", true);
            timeButton->object->setTexture(texture);
            timeButton->object->enableTexture(true);
            sceneTime = currentTime;
            std::cout << "Time set to " << currentTime << "\n";
        });
        keyframeButtons.push_back(timeButton->object);
        ++timeIndex;
    }

    yPos = yPos - (secondButtonSize + 0.02f) * (timeIndex / (int)(0.3f / secondButtonSize)) - 0.1f; 
    Texture *addKeyframeColor = new Texture("assets/textures/sidebar/KeyframedElementColor.jpg");
    Texture *addKeyframeButtonTexture = new Texture("assets\\textures\\sidebar\\AddKeyframeButton.jpg");
    SidebarElement* addKeyframeButton = createButton(xPos, yPos, 2, "AddKeyframeButton", glm::vec3(0.1f, 0.1f, 0.9f),0.3f, 0.1f, addKeyframeButtonTexture);
    addKeyframeButton->object->setOnClick([addKeyframeColor]()
    { 
        if(selectedObject)
        {
            selectedObject->addKeyframe(sceneTime);
            for(auto* elm : keyframeButtons)
            {
                if(elm->getName() == "TimeButton" + std::to_string((int)sceneTime))
                {
                    elm->setTexture(addKeyframeColor);
                    elm->enableTexture(true);
                }
            }
        }
    });

    yPos -= 0.15f;
    Texture *removeKeyframeButtonTexture = new Texture("assets\\textures\\sidebar\\DeleteKeyframeButton.jpg");
    SidebarElement* removeKeyframeButton = createButton(xPos, yPos, 2, "RemoveKeyframeButton", glm::vec3(0.0f, 0.0f, 0.0f), 0.3f, 0.1f, removeKeyframeButtonTexture);
    removeKeyframeButton->object->setOnClick([addKeyframeColor]()
    { 
        if(selectedObject)
        {
            selectedObject->removeKeyframe((int)sceneTime);
        }
    });


    yPos -= 0.15f;
    Texture *playButtonTexture = new Texture("assets\\textures\\sidebar\\PlayPauseButton.jpg");
    SidebarElement* playButton = createButton(xPos, yPos, 2, "PlayButton", glm::vec3(0.2f, 0.5f, 0.2f), 0.3f, 0.1f, playButtonTexture);
    playButton->object->setOnClick([]()
    { 
        play = !play; 
    });
}

SidebarElement* Sidebar::createArrow(float x, float y, int page, std::string name, const glm::vec3& color, float scale, Texture* texture, bool down) {
    std::vector<Tri> arrowRender;
    if(!down)
    {
        arrowRender.emplace_back(
            Vertex{{ x                  , y                 , 0.1f}, color, {0.0f, 1.0f}},
            Vertex{{ x + 0.05 * scale   , y                 , 0.1f}, color, {0.0f, 0.0f}},
            Vertex{{ x + 0.025f * scale , y + 0.05 * scale  , 0.1f}, color, {1.0f, 0.0f}}
        );
    }
    else
    {
        arrowRender.emplace_back(
            Vertex{{ x                  , y                 , 0.1f}, color, {0.0f, 1.0f}},
            Vertex{{ x + 0.05 * scale   , y                 , 0.1f}, color, {0.0f, 0.0f}},
            Vertex{{ x + 0.025f * scale , y - 0.05 * scale  , 0.1f}, color, {1.0f, 0.0f}}
        );
    }
    RenderableObjectStatic* arrowButton = new RenderableObjectStatic(arrowRender, shaderUI);
    arrowButton->position = glm::vec3(0.0f, 0.0f, 0.1f);
    arrowButton->setName(name);
    SidebarElement* elm = new SidebarElement{arrowButton, page};
    uiElements.push_back(elm);
    return elm;
}

SidebarElement* Sidebar::createButton(float x, float y, int page, std::string name, const glm::vec3& color, float width, float height, Texture* texture) {
    std::vector<Tri> buttonRender = {
        Tri(
            Vertex{{x         , y          , 0.0f}, color, {0.0f, 1.0f}},
            Vertex{{x + width , y          , 0.0f}, color, {1.0f, 1.0f}},
            Vertex{{x         , y - height , 0.0f}, color, {0.0f, 0.0f}}
        ),
        Tri(
            Vertex{{x         , y - height , 0.0f}, color, {0.0f, 0.0f}},
            Vertex{{x + width , y          , 0.0f}, color, {1.0f, 1.0f}},
            Vertex{{x + width , y - height , 0.0f}, color, {1.0f, 0.0f}}
        )
    };
    RenderableObjectStatic* button = new RenderableObjectStatic(buttonRender, shaderUI);
    button->position = glm::vec3(0.0f, 0.0f, 0.1f);
    button->setName(name);
    if(texture != nullptr)
    {
        button->setTexture(texture);
        button->enableTexture(true);
    }

    SidebarElement* elm = new SidebarElement{button, page};
    uiElements.push_back(elm);
    return elm;
}

//modify the visibility of certain ui elements based on the current page
void Sidebar::updateVisibility(GLFWwindow* window) {
    auto handleKeyPress = [&](int key, std::function<void()> onPress) {
        bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
        bool wasPressed = prevKeyStates[key];

        if (isPressed && !wasPressed) {
            onPress();  // key just pressed
        }

        prevKeyStates[key] = isPressed; // update state
    };

    handleKeyPress(GLFW_KEY_E, [&]() {
        if (currentPage < (TOTAL_PAGES - 1)) ++currentPage;
    });

    handleKeyPress(GLFW_KEY_Q, [&]() {
        if (currentPage > 0) --currentPage;
    });

    for (auto* elem : uiElements) {
        int elemPage = elem->page;
        elem->object->setVisible(elemPage == currentPage || elemPage == -1);
    }
}

void Sidebar::render() {
    // Disable depth for UI rendering
    glDisable(GL_DEPTH_TEST);
    for (auto* element : uiElements) {
        auto* obj = element->object;
        if (!obj) continue;
        if (obj->isVisible()) {
            obj->draw(glm::mat4(1.0f), std::vector<LightSource*>());
        }
    }
    glEnable(GL_DEPTH_TEST);
}