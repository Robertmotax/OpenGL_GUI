#pragma once
#include "RenderableObjectBase.h"

class LightSource;
class RenderableObjectStatic : public RenderableObjectBase {
public:
    RenderableObjectStatic(const std::vector<Tri> &triangles, Shader *shader);
    void draw(const glm::mat4& viewProj, const std::vector<LightSource*>& lights) const override;
    bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, const glm::mat4& viewProjInverse, float& outDistance) override;
    bool isDraggable() const override { return false; }

    // For proper pagination
    void setVisible(bool v);
    bool isVisible() const;

private:
    GLuint useTextureLocation = -1;
    GLuint textureSamplerLocation = -1;
    GLuint isNDC_Location = -1;
    GLuint uVP_Location = -1;
    GLuint uModel_Location = -1;

    //For proper pagination
    bool visible = true;

};
