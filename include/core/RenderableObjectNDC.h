#pragma once
#include "RenderableObjectBase.h"

class RenderableObjectNDC : public RenderableObjectBase {
public:
    RenderableObjectNDC(const std::vector<Tri> &triangles, Shader *shader);
    void draw(const glm::mat4& viewProj) const override;
    bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight) override;

private:
};
