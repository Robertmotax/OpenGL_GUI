// RenderableObjectNDC.cpp
#include "core/RenderableObjectStatic.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class LightSource;
RenderableObjectStatic::RenderableObjectStatic(const std::vector<Tri>& triangles, Shader* shader)
    : RenderableObjectBase(triangles, shader)
{
    name = "staticObj" + id;
    // Cache uniform locations once when the object is created
    useTextureLocation = glGetUniformLocation(shader->getID(), "useTexture");
    textureSamplerLocation = glGetUniformLocation(shader->getID(), "textureSampler");
    isNDC_Location = glGetUniformLocation(shader->getID(), "isNDC");
    uVP_Location = glGetUniformLocation(shader->getID(), "uVP");
    uModel_Location = glGetUniformLocation(shader->getID(), "uModel");

}

void RenderableObjectStatic::draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    shader->use();

    // If texture is not used, set uniform to false
    glUniform1i(useTextureLocation, useTexture ? 1 : 0);

    // Set up texture if enabled and available
    if (useTexture && texture) {
        texture->useTexture();
        glUniform1i(textureSamplerLocation, 0);
    }

    
    glm::mat4 identity = glm::mat4(1.0f);
    glUniform1i(isNDC_Location, 1);
    glUniformMatrix4fv(uVP_Location, 1, GL_FALSE, glm::value_ptr(identity));
    glUniformMatrix4fv(uModel_Location, 1, GL_FALSE, glm::value_ptr(identity));

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);     // Restore depth testing
    glDepthMask(GL_TRUE);        // Restore depth writing
}


bool RenderableObjectStatic::isClicked(float mouseX, float mouseY, int winWidth, int winHeight, const glm::mat4& viewProjInverse, float& outDistance)
{
    // Convert mouse coordinates to NDC
    float x_ndc = (2.0f * mouseX) / winWidth - 1.0f;
    float y_ndc = 1.0f - (2.0f * mouseY) / winHeight;
    glm::vec2 p(x_ndc, y_ndc);

    // //Debug output
    // std::cout << "x pisition: " << mouseX << ", y position: " << mouseY << std::endl;

    for (const auto& tri : tris) {
        glm::vec2 a(tri.v0.position.x, tri.v0.position.y);
        glm::vec2 b(tri.v1.position.x, tri.v1.position.y);
        glm::vec2 c(tri.v2.position.x, tri.v2.position.y);

        glm::vec2 v0 = b - a;
        glm::vec2 v1 = c - a;
        glm::vec2 v2 = p - a;

        float d00 = glm::dot(v0, v0);
        float d01 = glm::dot(v0, v1);
        float d11 = glm::dot(v1, v1);
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);

        float denom = d00 * d11 - d01 * d01;
        if (denom == 0.0f) continue; // Degenerate triangle

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        if ((u >= 0) && (v >= 0) && (w >= 0))
            return true;
    }

    return false;
}
