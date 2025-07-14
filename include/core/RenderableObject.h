// RenderableObject.h
#pragma once
#include "RenderableObjectBase.h"

class RenderableObject : public RenderableObjectBase {
public:
    RenderableObject(const std::vector<Tri> &triangles, Shader *shader);
    void draw(const glm::mat4& viewProj) const override;

private:
};
