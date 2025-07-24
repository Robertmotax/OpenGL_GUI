#include "core/RenderableObject.h"
#include <iostream>
#include "core/Vertex.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>


RenderableObject::RenderableObject(const std::vector<Tri>& triangles, Shader* shader, Shader* shaderShadow)
    : RenderableObjectBase(triangles, shader)
{
    this->shaderShadow = shaderShadow;
}

void RenderableObject::draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const {
    shader->use();

    glm::mat4 model = getModelMatrix();; // Use your actual transform

    GLuint shaderID = shader->getID();

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uVP"), 1, GL_FALSE, glm::value_ptr(viewProj));

    for (int i = 0; i < (int)lights.size(); ++i) {
    // Set up texture if enabled and available
        if (useTexture && texture) {
            texture->useTexture(); // bind only when required
            glUniform1i(glGetUniformLocation(shader->getID(), "uUseTexture"), 0);
        }
        // If texture is not used, set uniform to false
        glUniform1i(glGetUniformLocation(shader->getID(), "uUseTexture"), useTexture ? 1 : 0); 
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}


void RenderableObject::drawDepthOnly(glm::mat4 lightSpaceMatrix) const {
    shaderShadow->use();
    glPolygonOffset(1.5f, 3.0f);    
    glm::mat4 model = getModelMatrix();
    shaderShadow->setMat4("uModel", model);
    glUniformMatrix4fv(glGetUniformLocation(shaderShadow->getID(), "uLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

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
