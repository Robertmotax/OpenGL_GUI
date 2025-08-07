// RenderableObject.h
#pragma once
#include "core/RenderableObjectBase.h"
#include "core/LightSource.h"
#include "Keyframe.h"

class LightSource;
class RenderableObject : public RenderableObjectBase {
public:
    RenderableObject(const std::vector<Tri> &triangles, Shader *shader, Shader *shadowShader, bool unlit = false);
    void draw(const glm::mat4& viewProj, const std::vector<LightSource*>& lights) const override;
    bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, const glm::mat4& viewProjInverse, float& outDistance) override;
    void drawDepthOnly(const glm::mat4& shadowMatrix, const glm::vec3& lightPos, float farPlane) const;
    bool getIsUnlit() const { return isUnlit; }
    virtual bool isRayIntersecting(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& outDistance);
    // Draggable by the mouse click handler and move it according to the mouse position
    bool isDraggable() const override { return true; }  // default: draggable

    //Hierarchical Modelling function 
    void setParent(RenderableObject* newParent);
    glm::mat4 getLocalTransform() const { return localTransform; }
    void setLocalTransform(const glm::mat4& localT) { localTransform = localT; }
    void updateSelfAndChildren();
    void updateFromKeyframes(float currentTime);

    //Delete object from the screen
    void deleteObject();
    void detachFromParent();
    void cleanupRemainingData();
    bool isUnlit;


    private:
        Shader *shaderShadow;
        std::vector<Keyframe> keyframes;
        RenderableObject* parent = nullptr;
        std::vector<RenderableObject*> children;
        glm::mat4 localTransform = glm::mat4(1.0f);
};
