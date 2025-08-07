// Globals.h
#pragma once

#include <vector>
#include "core/RenderableObject.h"  // Make sure this is the correct include path for RenderableObject

extern std::vector<RenderableObjectBase*> allObjects;
extern std::vector<LightSource*> lights;
extern RenderableObject* selectedObject;
extern Shader* defaultShader;
extern Shader* shadowShader;
extern bool waitingForParentSelection;
extern int sceneTime;
extern std::unordered_map<int, bool> prevKeyStates;
