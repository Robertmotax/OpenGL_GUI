// Globals.h
#pragma once

#include <vector>
#include "core/RenderableObject.h"
#include "core/RenderableObjectStatic.h"
#include "ui/Sidebar.h"

extern std::vector<RenderableObjectBase*> allObjects;
extern std::vector<SidebarElement*> uiElements;
extern std::vector<RenderableObjectStatic*> keyframeButtons;
extern std::vector<LightSource*> lights;
extern RenderableObject* selectedObject;
extern Shader* defaultShader;
extern Shader* shadowShader;
extern bool waitingForParentSelection;
extern float sceneTime;
extern float lastTime;
extern bool play;
extern std::unordered_map<int, bool> prevKeyStates;

void setSelectedObject(RenderableObject *obj);
