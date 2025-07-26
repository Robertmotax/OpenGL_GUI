// RenderableObject.h
#pragma once
#include "RenderableObjectBase.h"

class RenderableObject : public RenderableObjectBase {
    public:
        RenderableObject(const std::vector<Tri> &triangles, Shader *shader, Shader *shadowShader);
        void draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const override;
        bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, const glm::mat4& viewProjInverse, float& outDistance) override;
        void drawDepthOnly(const glm::mat4& shadowMatrix, const glm::vec3& lightPos, float farPlane) const;
        virtual bool isRayIntersecting(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& outDistance);
        // Draggable by the mouse click handler and move it according to the mouse position
        bool isDraggable() const override { return true; }  // default: draggable

    private:
        Shader *shaderShadow;

};
