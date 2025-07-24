// RenderableObject.h
#pragma once
#include "RenderableObjectBase.h"

class RenderableObject : public RenderableObjectBase {
    public:
        RenderableObject(const std::vector<Tri> &triangles, Shader *shader, Shader *shadowShader);
        void draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const override;
        bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, glm::mat4 viewProjInverse) override;
        void drawDepthOnly() const;
        virtual bool isRayIntersecting(const glm::vec3& rayOrigin, const glm::vec3& rayDirection);

        //Getters and setters
        void setPosition(const glm::vec3& pos);
        glm::vec3 getPosition() const { return position; };

    private:
        Shader *shaderShadow;
        glm::vec3 position;

};
