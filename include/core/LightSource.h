// core/LightSource.h
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <GL/glew.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

struct LightSource {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;

    // Shadow mapping
    GLuint shadowMapFBO = 0;
    GLuint shadowMapTex = 0;
    glm::mat4 lightSpaceMatrix; // For directional shadow mapping

    // Shadow map settings
    const GLuint SHADOW_WIDTH = 1024;
    const GLuint SHADOW_HEIGHT = 1024;

    LightSource(const glm::vec3& pos, const glm::vec3 dir, const glm::vec3& col, float inten)
        : position(pos), direction(dir), color(col), intensity(inten) {}

    void initShadowMap() {
        // 1. Generate framebuffer and depth texture
        glGenFramebuffers(1, &shadowMapFBO);
        glGenTextures(1, &shadowMapTex);

        glBindTexture(GL_TEXTURE_2D, shadowMapTex);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT,
            0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
        );

        // 2. Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Use GL_LINEAR for PCF
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 }; // not in shadow
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        // 3. Attach depth texture to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTex, 0);

        // No color buffer is drawn/read
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // 4. Check completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Error: Shadow framebuffer is not complete!" << std::endl;
        }

        // 5. Cleanup bindings
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    // Computes light space matrix for a directional-like shadow (from this light’s POV)
    void computeLightSpaceMatrix() {
        glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 75.0f);
        glm::vec3 dir = glm::normalize(direction);
        glm::mat4 lightView = glm::lookAt(position,
                                        position + direction,
                                        glm::vec3(0.0, 1.0, 0.0));
        if (glm::length(direction) < 0.001f) {
            std::cerr << "WARNING: Light direction is zero!" << std::endl;
        }
        //std::cout << "lightView = " << glm::to_string(lightView) << std::endl;
        lightSpaceMatrix = lightProjection * lightView;
    }
};
