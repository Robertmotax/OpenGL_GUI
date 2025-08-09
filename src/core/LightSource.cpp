#include "core/LightSource.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>
#include "core/RenderableObject.h"
#include "core/util.h"
#include <Globals.h>

int LightSource::lastId = 0;

LightSource::LightSource(const glm::vec3& pos, const glm::vec3& col, const glm::vec3& inten, Shader* shader, Shader* shaderShadow, float farP)
    : farPlane(farP), id(lastId++) {
    
    lightHandler = new RenderableObject(makeCube(glm::vec3(0.0f), inten, glm::vec3(0.8f)), shader, shaderShadow, true);
    lightHandler->position = pos;
    lightHandler->rotation = col;
    lightHandler->scale = glm::vec3(inten);
    lightHandler->updateLocalTransformFromComponents();
    lightHandler->updateSelfAndChildren();

    position = &lightHandler->position;
    color = &lightHandler->rotation;
    intensity = &lightHandler->scale;
}

LightSource::~LightSource() {
    glDeleteFramebuffers(1, &shadowMapFBO);
    glDeleteTextures(1, &shadowCubemap);
}

void LightSource::initShadowCubemap() {
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

void LightSource::computeShadowTransforms() {
    shadowTransforms.clear();
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);

    glm::vec3 pos = *position;
    shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)));
}

void LightSource::renderShadowMap() {
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    computeShadowTransforms();

    for (int face = 0; face < 6; ++face) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, shadowCubemap, 0);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (auto* obj : allObjects)
        {
            if(auto* sceneObj = dynamic_cast<RenderableObject*>(obj))
            {
                if(!sceneObj->isUnlit)
                {
                    sceneObj->drawDepthOnly(shadowTransforms[face], *position, farPlane);
                }
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Optional: debug OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}
