// RenderableObjectNDC.cpp
#include "core/RenderableObjectNDC.h"
#include <glm/gtc/type_ptr.hpp>

RenderableObjectNDC::RenderableObjectNDC(const std::vector<Tri>& triangles, Shader* shader)
    : RenderableObjectBase(triangles, shader)
{
    // No additional initialization needed.
}

void RenderableObjectNDC::draw(const glm::mat4& viewProj) const {
    shader->use();

    // Set identity matrix for NDC rendering
    glm::mat4 identity = glm::mat4(1.0f);
    GLuint uVP = glGetUniformLocation(shader->getID(), "uVP");
    glUniformMatrix4fv(uVP, 1, GL_FALSE, glm::value_ptr(identity));

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

bool RenderableObjectNDC::isClicked(float mouseX, float mouseY, int winWidth, int winHeight) {
    // Convert mouse coords to NDC
    float x_ndc = (2.0f * mouseX) / winWidth - 1.0f;
    float y_ndc = 1.0f - (2.0f * mouseY) / winHeight;
    
    for(const auto& tri : trianglePlanes)
    {
        if(tri.isClicked(x_ndc, y_ndc))
            return true;
    }
    return false;
}
