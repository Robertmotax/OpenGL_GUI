#pragma once
#include "RenderableObjectBase.h"

class RenderableObjectStatic : public RenderableObjectBase {
public:
    RenderableObjectStatic(const std::vector<Tri> &triangles, Shader *shader);
    void draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const override;
    bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, glm::mat4 viewProjInverse) override;

private:
};
