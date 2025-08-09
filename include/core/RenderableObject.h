// RenderableObject.h
#pragma once
#include "core/RenderableObjectBase.h"
#include "core/LightSource.h"
#include "Keyframe.h"
#include <algorithm>
#include <glm/gtx/matrix_decompose.hpp>

class LightSource;
class RenderableObject : public RenderableObjectBase {
public:
    RenderableObject(const std::vector<Tri> &triangles, Shader *shader, Shader *shadowShader, bool unlit = false);
    void draw(const glm::mat4& viewProj, const std::vector<LightSource*>& lights, glm::vec3 cameraPos) const override;
    bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, const glm::mat4& viewProjInverse, float& outDistance) override;
    void drawDepthOnly(const glm::mat4& shadowMatrix, const glm::vec3& lightPos, float farPlane) const;
    bool getIsUnlit() const { return isUnlit; }
    virtual bool isRayIntersecting(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& outDistance);
    // Draggable by the mouse click handler and move it according to the mouse position
    bool isDraggable() const override { return true; }  // default: draggable

    //Hierarchical Modelling function 
    RenderableObject* parent = nullptr;
    void setParent(RenderableObject* newParent);
    void updateSelfAndChildren();

    std::vector<Keyframe> keyframes = {};
    void updateFromKeyframes(float currentTime);
    void addKeyframe(float time) {
        removeKeyframe(time);
        glm::vec3 extractedPosition, extractedScale;
        glm::quat extractedRotation;

        // Decompose the localTransform matrix into components
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(localTransform, extractedScale, extractedRotation, extractedPosition, skew, perspective);
        glm::vec3 eulerRotation = glm::eulerAngles(extractedRotation);

        keyframes.emplace_back(time, extractedPosition, eulerRotation, extractedScale);

        std::sort(keyframes.begin(), keyframes.end(), [](const Keyframe &a, const Keyframe &b) {
            return a.time < b.time;
        });

        std::cout << "Added keyframe at time " << time
          << " pos: " << glm::to_string(extractedPosition)
          << " rot(rad): " << glm::to_string(eulerRotation)
          << " scale: " << glm::to_string(extractedScale) << "\n";

        for (const auto& kf : keyframes) {
            std::cout << "Keyframe: t=" << kf.time
                    << " pos=" << glm::to_string(kf.position)
                    << " rot=" << glm::to_string(kf.rotation)
                    << " scale=" << glm::to_string(kf.scale) << "\n";
}
    }

    void removeKeyframe(float time) {
        keyframes.erase(
            std::remove_if(keyframes.begin(), keyframes.end(),
                        [time](const auto& kf) { return kf.time == time; }),
            keyframes.end()
        );
    }

    void updateLocalTransformFromComponents() {
        localTransform = glm::translate(glm::mat4(1.0f), position) *
                        glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1,0,0)) *
                        glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0,1,0)) *
                        glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0,0,1)) *
                        glm::scale(glm::mat4(1.0f), scale);
    }

    //Delete object from the screen
    void deleteObject();
    void detachFromParent();
    void cleanupRemainingData();
    bool isUnlit;


    private:
        Shader *shaderShadow;
        std::vector<RenderableObject*> children;
        glm::mat4 localTransform = glm::mat4(1.0f);
};
