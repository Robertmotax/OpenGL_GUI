#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>
#include <iostream>

struct LightSource {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float farPlane;

    // Shadow mapping
    GLuint shadowMapFBO = 0;
    GLuint shadowCubemap = 0;
    std::vector<glm::mat4> shadowTransforms;

    // Shadow map settings
    const GLuint SHADOW_WIDTH = 1024;
    const GLuint SHADOW_HEIGHT = 1024;

    LightSource(const glm::vec3& pos, const glm::vec3& col, float inten, float farP = 25.0f)
        : position(pos), color(col), intensity(inten), farPlane(farP) {}

    void initShadowCubemap() {
        // Create cubemap texture
        glGenTextures(1, &shadowCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubemap);
        for (GLuint i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                         SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // Create framebuffer
        glGenFramebuffers(1, &shadowMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Error: Point light shadow framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void computeShadowTransforms() {
        shadowTransforms.clear();
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);

        shadowTransforms.push_back(shadowProj *
            glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0),  glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0),  glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0),  glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
    }
};
