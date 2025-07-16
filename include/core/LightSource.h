// core/LightSource.h
#pragma once
#include <glm/glm.hpp>

struct LightSource {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

    GLuint shadowMapFBO;
    GLuint shadowMapTex;
    glm::mat4 lightSpaceMatrix;

    LightSource(const glm::vec3& pos, const glm::vec3& col, float inten)
    : position(pos), color(col), intensity(inten) {}

    void initShadowMap() {
        const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

        glGenFramebuffers(1, &shadowMapFBO);
        glGenTextures(1, &shadowMapTex);
        glBindTexture(GL_TEXTURE_2D, shadowMapTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTex, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

