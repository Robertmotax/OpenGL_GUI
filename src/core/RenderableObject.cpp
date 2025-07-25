#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "core/RenderableObject.h"
#include <iostream>
#include "core/Vertex.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include "core/util.h"

RenderableObject::RenderableObject(const std::vector<Tri>& triangles, Shader* shader, Shader* shaderShadow)
    : RenderableObjectBase(triangles, shader)
{
    this->shaderShadow = shaderShadow;
}

void RenderableObject::draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const {
    shader->use();

    glm::mat4 model = getModelMatrix();
    GLuint shaderID = shader->getID();

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uVP"), 1, GL_FALSE, glm::value_ptr(viewProj));

    // --- Texture 2D binding ---
    if (useTexture && texture) {
        glUniform1i(glGetUniformLocation(shaderID, "uUseTexture"), 1);
        texture->useTexture();
        glUniform1i(glGetUniformLocation(shaderID, "uTexture"), 0);
    } else {
        glUniform1i(glGetUniformLocation(shaderID, "uUseTexture"), 0);
    }

    // --- Shadow cubemap lights ---
    glUniform1i(glGetUniformLocation(shaderID, "uNumLights"), (int)lights.size());
    
    // Start from stored position
    glm::mat4 modelTransformed = glm::translate(model, position);

    for (int i = 0; i < (int)lights.size(); ++i) {
        const LightSource& light = lights[i];

        std::string idx = std::to_string(i);
        glUniform3fv(glGetUniformLocation(shaderID, ("lightPositions[" + idx + "]").c_str()), 1, glm::value_ptr(light.position));
        glUniform3fv(glGetUniformLocation(shaderID, ("lightColors[" + idx + "]").c_str()), 1, glm::value_ptr(light.color));
        glUniform1f(glGetUniformLocation(shaderID, ("lightIntensities[" + idx + "]").c_str()), light.intensity);
        glUniform1f(glGetUniformLocation(shaderID, ("farPlanes[" + idx + "]").c_str()), light.farPlane);

        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadowCubemap);
        glUniform1i(glGetUniformLocation(shaderID, ("shadowMaps[" + idx + "]").c_str()), 1 + i);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    // Optional: debug OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}


void RenderableObject::drawDepthOnly(const glm::mat4& shadowMatrix, const glm::vec3& lightPos, float farPlane) const {
    if (!shaderShadow) {
        std::cerr << "shaderShadow is null!\n";
        return;
    }
    //std::cout << glm::to_string(shadowMatrix) << std::endl;

    shaderShadow->use();

    //std::cout << "Setting uShadowMatrix uniform\n";
    shaderShadow->setMat4("uShadowMatrix", shadowMatrix);

    //std::cout << "Setting uFarPlane uniform\n";
    shaderShadow->setFloat("uFarPlane", farPlane);

    //std::cout << "Setting uLightPos uniform\n";
    shaderShadow->setVec3("uLightPos", lightPos);

    //std::cout << "Setting uModel uniform\n";
    glm::mat4 modelMatrix = getModelMatrix();
    shaderShadow->setMat4("uModel", modelMatrix);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}


bool RenderableObject::isClicked(float mouseX, float mouseY, int winWidth, int winHeight, glm::mat4 viewProjInverse) {
    // 1. Convert screen space to normalized device coordinates (NDC)
    float x = (2.0f * mouseX) / winWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / winHeight; // Invert Y
    glm::vec4 ndcNear(x, y, -1.0f, 1.0f);
    glm::vec4 ndcFar(x, y, 1.0f, 1.0f);

    // 2. Convert to world space using inverse view-projection
    glm::vec4 worldNear = viewProjInverse * ndcNear;
    glm::vec4 worldFar = viewProjInverse * ndcFar;
    worldNear /= worldNear.w;
    worldFar /= worldFar.w;

    glm::vec3 rayOrigin = glm::vec3(worldNear);
    glm::vec3 rayDir = glm::normalize(glm::vec3(worldFar - worldNear));

    // 3. Ray-triangle intersection test
    for (const Tri& tri : tris) {
        const glm::vec3& v0 = tri.v0.position;
        const glm::vec3& v1 = tri.v1.position;
        const glm::vec3& v2 = tri.v2.position;

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 h = glm::cross(rayDir, edge2);
        float a = glm::dot(edge1, h);
        if (fabs(a) < 1e-6f) continue;  // Parallel

        float f = 1.0f / a;
        glm::vec3 s = rayOrigin - v0;
        float u = f * glm::dot(s, h);
        if (u < 0.0f || u > 1.0f) continue;

        glm::vec3 q = glm::cross(s, edge1);
        float v = f * glm::dot(rayDir, q);
        if (v < 0.0f || u + v > 1.0f) continue;

        float t = f * glm::dot(edge2, q);
        if (t > 0.001f) return true; // Intersection
    }

    return false;
}


bool RenderableObject::isRayIntersecting(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) {
    // Compute AABB from model-transformed bounding box
    glm::vec3 minBounds(FLT_MAX), maxBounds(-FLT_MAX);

    for (const Tri& tri : tris) {
        for (const glm::vec3& vertex : { tri.v0.position, tri.v1.position, tri.v2.position }) {
            glm::vec4 worldPos = model * glm::vec4(vertex, 1.0f);
            glm::vec3 wp = glm::vec3(worldPos);
            minBounds = glm::min(minBounds, wp);
            maxBounds = glm::max(maxBounds, wp);
        }
    }

    float t;
    return rayIntersectsAABB(rayOrigin, rayDirection, minBounds, maxBounds, t);
}
