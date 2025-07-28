#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "core/Shader.h"

class RenderableObject;

class LightSource {
public:
    LightSource(const glm::vec3& pos, const glm::vec3& col, const glm::vec3& inten, Shader* shader, Shader* shaderShadow, float farP = 25.0f);
    ~LightSource();  // Destructor for cleanup

    void initShadowCubemap();
    void computeShadowTransforms();
    void renderShadowMap(std::vector<RenderableObject*> sceneObjects);

    const std::vector<glm::mat4>& getShadowTransforms() const { return shadowTransforms; }
    GLuint getShadowCubemap() const { return shadowCubemap; }
    GLuint getShadowMapFBO() const { return shadowMapFBO; }

    glm::vec3* position;
    glm::vec3* color;
    glm::vec3* intensity;

    float farPlane;
    int id;

    static constexpr GLuint SHADOW_WIDTH = 1024;
    static constexpr GLuint SHADOW_HEIGHT = 1024;
    RenderableObject* lightHandler = nullptr;

private:
    GLuint shadowMapFBO = 0;
    GLuint shadowCubemap = 0;
    std::vector<glm::mat4> shadowTransforms;

    static int lastId;
};
