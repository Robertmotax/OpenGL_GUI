// Globals.cpp
#include "Globals.h"

std::vector<RenderableObjectBase*> allObjects;
std::vector<LightSource*> lights;
RenderableObject* selectedObject;
Shader* defaultShader;
Shader* shadowShader;
