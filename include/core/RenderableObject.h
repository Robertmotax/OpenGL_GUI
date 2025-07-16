// RenderableObject.h
#pragma once
#include "RenderableObjectBase.h"

class RenderableObject : public RenderableObjectBase {
    public:
        RenderableObject(const std::vector<Tri> &triangles, Shader *shader, Shader *shadowShader);
        void draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const override;
        bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, glm::mat4 viewProjInverse) override;
        void drawDepthOnly() const;


    private:
        Shader *shaderShadow;
};
