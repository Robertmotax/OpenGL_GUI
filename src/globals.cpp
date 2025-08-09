// Globals.cpp
#include "Globals.h"

std::vector<RenderableObjectBase*> allObjects;
std::vector<SidebarElement*> uiElements;
std::vector<RenderableObjectStatic*> keyframeButtons;
std::vector<LightSource*> lights;
RenderableObject* selectedObject;
Shader* defaultShader;
Shader* shadowShader;
bool waitingForParentSelection;
float sceneTime;
float lastTime;
bool play;
std::unordered_map<int, bool> prevKeyStates;

void setSelectedObject(RenderableObject *obj)
{
    selectedObject = obj;
    Texture *keyframePresentColor = new Texture("assets/textures/KeyframedElementColor.jpg");
    for(auto* keyButton : keyframeButtons)
    {
        if(keyButton->texture && keyButton->texture->fileLocation == "assets/textures/KeyframedElementColor.jpg")
        {
            keyButton->setTexture(nullptr);
            keyButton->enableTexture(false);
        }
    }
    for(auto key : selectedObject->keyframes)
    {
        for(auto* keyButton : keyframeButtons)
        {
            if(keyButton->getName() == "TimeButton" + std::to_string((int)key.time))
            {
                if(!(keyButton->texture && keyButton->texture->fileLocation == "assets/textures/SelectedElementColor.jpg"))
                {
                    keyButton->setTexture(keyframePresentColor);
                    keyButton->enableTexture(true);
                }
            }
        }
    }
    std::cout << "Selected obj is now " << obj->getName();
}


