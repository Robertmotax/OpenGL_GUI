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
    //Set clickable objects for translation
    float xStart = -1.0 + PADDING;
    yStart -= 0.05f;
    SidebarElement *transXUp = createArrow(xStart, yStart, 0, "translateXUp", green);
    SidebarElement *transXDown = createArrow(xStart, yStart, 0, "translateXDown", red, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *transYUp = createArrow(xStart, yStart, 0, "translateYUp", green);
    SidebarElement *transYDown = createArrow(xStart, yStart, 0, "translateYDown", red, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *transZUp = createArrow(xStart, yStart, 0, "translateZUp", green);
    SidebarElement *transZDown = createArrow(xStart, yStart, 0, "translateZDown", red, nullptr, true);

    transXUp->object->setOnClick([&](){ if(selectedObject) selectedObject->position.x += 0.05; });
    transXDown->object->setOnClick([&](){ if(selectedObject) selectedObject->position.x -= 0.05; });
    transYUp->object->setOnClick([&](){ if(selectedObject) selectedObject->position.y += 0.05; });
    transYDown->object->setOnClick([&](){ if(selectedObject) selectedObject->position.y -= 0.05; });
    transZUp->object->setOnClick([&](){ if(selectedObject) selectedObject->position.z += 0.05; });
    transZDown->object->setOnClick([&](){ if(selectedObject) selectedObject->position.z -= 0.05; });


    //Set clickable objects for rotation
    yStart -= 0.20;
    xStart = -1.0 + PADDING;
    SidebarElement *rotXUp = createArrow(xStart, yStart, 0, "rotateXUp", green);
    SidebarElement *rotXDown = createArrow(xStart, yStart, 0, "rotateXDown", red, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *rotYUp = createArrow(xStart, yStart, 0, "rotateYUp", green);
    SidebarElement *rotYDown = createArrow(xStart, yStart, 0, "rotateYDown", red, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *rotZUp = createArrow(xStart, yStart, 0, "rotateZUp", green);
    SidebarElement *rotZDown = createArrow(xStart, yStart, 0, "rotateZDown", red, nullptr, true);

    rotXUp->object->setOnClick([&](){ if(selectedObject) selectedObject->rotation.x += 0.05; });
    rotXDown->object->setOnClick([&](){ if(selectedObject) selectedObject->rotation.x -= 0.05; });
    rotYUp->object->setOnClick([&](){ if(selectedObject) selectedObject->rotation.y += 0.05; });
    rotYDown->object->setOnClick([&](){ if(selectedObject) selectedObject->rotation.y -= 0.05; });
    rotZUp->object->setOnClick([&](){ selectedObject->rotation.z += 0.05; });
    rotZDown->object->setOnClick([&](){ if(selectedObject) selectedObject->rotation.z -= 0.05; });

    //Set clickable objects for scale
    yStart -= 0.20;
    xStart = -1.0 + PADDING;
    SidebarElement *sclXUp = createArrow(xStart, yStart, 0, "scaleXUp", green);
    SidebarElement *sclXDown = createArrow(xStart, yStart, 0, "scaleXDown", red, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *sclYUp = createArrow(xStart, yStart, 0, "scaleYUp", green);
    SidebarElement *sclYDown = createArrow(xStart, yStart, 0, "scaleYDown", red, nullptr, true);
    xStart += (SIDEBAR_WIDTH - 2 * PADDING) / 3;
    SidebarElement *sclZUp = createArrow(xStart, yStart, 0, "scaleZUp", green);
    SidebarElement *sclZDown = createArrow(xStart, yStart, 0, "scaleZDown", red, nullptr, true);
    
    sclXUp->object->setOnClick([&](){ if(selectedObject) selectedObject->scale.x += 0.05; });
    sclXDown->object->setOnClick([&](){ if(selectedObject) selectedObject->scale.x -= 0.05; });
    sclYUp->object->setOnClick([&](){ if(selectedObject) selectedObject->scale.y += 0.05; });
    sclYDown->object->setOnClick([&](){ if(selectedObject) selectedObject->scale.y -= 0.05; });
    sclZUp->object->setOnClick([&](){ if(selectedObject) selectedObject->scale.z += 0.05; });
    sclZDown->object->setOnClick([&](){ if(selectedObject) selectedObject->scale.z -= 0.05; });

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
    SidebarElement* spawnCubeButton = createButton(xPos, yPos, 1, "SpawnCubeButton", glm::vec3(0.0f, 0.0f, 1.0f));
    spawnCubeButton->object->setOnClick([]()
    { 
        allObjects.push_back(new RenderableObject(generateCubeTris(1.0f, glm::vec3(0.5f)), defaultShader, shadowShader));
    });

    // Spawn Light Button
    yPos -= 0.15;
    SidebarElement* spawnLightButton = createButton(xPos, yPos, 1, "SpawnLightButton", glm::vec3(0.9f));
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
    SidebarElement* deleteButton = createButton(xPos, yPos, 1, "DeleteObjectButton", glm::vec3(0.0f));
    deleteButton->object->setOnClick([this]() {
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
            objLoader.importFromObj(path);
        });

        ++index;
    }

    //Button to save scene
    yPos = -0.9;
    SidebarElement* loaderButton = createButton(xPos, yPos, 1, "loader" + std::to_string(index), glm::vec3(0.0f, 1.0f, 0.0f));

    loaderButton->object->setOnClick([this, index, columnCount, xPos, yPosLoaders, tileSize, rgb]() mutable {
        OBJLoader objLoader = OBJLoader();
        std::string filename = objLoader.getAvailableFilename("assets/models", "unnamed", ".obj");
        objLoader.exportToObj(filename);
        std::cout << "Saved Scene " << std::endl;

        int row = index / columnCount;
        int col = index % columnCount;

        float xStart = xPos + col * tileSize;
        float yStart = yPosLoaders - row * tileSize;
        glm::vec3 color = (float)row * glm::vec3(0.1f) + rgb[index % columnCount] + glm::vec3(0.4f);

        SidebarElement* loaderButton = createButton(xStart, yStart, 1, "loader" + std::to_string(index), color, tileSize, tileSize);

        loaderButton->object->setOnClick([filename]() {
            OBJLoader objLoader = OBJLoader();
            objLoader.importFromObj("models/assets/" + filename + ".obj");
        });

        ++index;
    });
}

void Sidebar::createAnimationButtons(float xPos, float yPos)
{
    // Add animation buttons
    float secondButtonSize = 0.02f;
    int numberOfSeconds = 60;
    int timeIndex = 0;

    while (timeIndex < numberOfSeconds) {
        int currentTime = timeIndex;
        float xStart = xPos + (currentTime % ((int)(0.3f / secondButtonSize))) * secondButtonSize;
        float yStart = yPos - (secondButtonSize + 0.02f) * (currentTime / (int)(0.3f / secondButtonSize));

        glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);
        SidebarElement* loaderButton = createButton(xStart, yStart, 2, "TimeButton" + std::to_string(currentTime), color, secondButtonSize, secondButtonSize);

        loaderButton->object->setOnClick([loaderButton, currentTime]() {
            for(auto* elm : allObjects)
            {
                if(elm->getName() == "TimeButton" + std::to_string(sceneTime))
                {
                    elm->setTexture(nullptr);
                    elm->enableTexture(false);
                }
            }
            Texture* texture = new Texture("assets/textures/SelectedElementColor.jpg", true);
            loaderButton->object->setTexture(texture);
            loaderButton->object->enableTexture(true);
            sceneTime = currentTime;
            std::cout << "Time set to " << currentTime;
        });
        keyframeButtons.push_back(loaderButton->object);
        ++timeIndex;
    }

    yPos = yPos - (secondButtonSize + 0.02f) * (timeIndex / (int)(0.3f / secondButtonSize)) - 0.1f; 
    Texture *addKeyframeColor = new Texture("assets/textures/AddKeyframeColor.jpeg");
    SidebarElement* addKeyframeButton = createButton(xPos, yPos, 2, "AddKeyframeButton", glm::vec3(0.1f, 0.1f, 0.9f));
    addKeyframeButton->object->setOnClick([]()
    { 
        selectedObject->addKeyframe(sceneTime); 
    });
}

SidebarElement* Sidebar::createArrow(float x, float y, int page, std::string name, const glm::vec3& color, Texture* texture, bool down) {
    std::vector<Tri> arrowRender;
    if(!down)
    {
        arrowRender.emplace_back(
            Vertex{{ x , y, 0.1f}, color, {0.0f, 1.0f}},
            Vertex{{ x + 0.05 , y, 0.1f}, color, {0.0f, 0.0f}},
            Vertex{{ x + 0.025f, y + 0.05, 0.1f}, color, {1.0f, 0.0f}}
        );
    }
    else
    {
        arrowRender.emplace_back(
            Vertex{{ x , y, 0.1f}, color, {0.0f, 1.0f}},
            Vertex{{ x + 0.05 , y, 0.1f}, color, {0.0f, 0.0f}},
            Vertex{{ x + 0.025f, y - 0.05, 0.1f}, color, {1.0f, 0.0f}}
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

void Sidebar::setSelectedObject(RenderableObject* obj) {
    selectedObject = obj;
        Texture *keyframePresentColor = new Texture("assets/textures/AddKeyframeColor.jpeg");
    for(auto key : selectedObject->keyframes)
    {
        for(auto* keyButtons : keyframeButtons)
        {
            if(keyButtons->getName() == "TimeButton" + std::to_string(key.time))
            {
                keyButtons->setTexture(keyframePresentColor);
            }
        }
    }
    std::cout << "Selected obj is now " << obj->getName();
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