#include "core/RenderableObject.h"
#include <iostream>
#include "core/Vertex.h"

RenderableObject::RenderableObject(const std::vector<Tri>& triangles, Shader* shader)
    : RenderableObjectBase(triangles, shader)
{
    // No additional initialization needed.
}

void RenderableObjectBase::draw(const glm::mat4& viewProj) const {
    shader->use();
    shader->setMat4("uVP", viewProj);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

bool RenderableObjectBase::isClicked(float mouseX, float mouseY, int winWidth, int winHeight) {
    glm::vec4 screen = glm::vec4(mouseX / winWidth * 2.0f - 1.0f,
                             1.0f - mouseY / winHeight * 2.0f,
                             0.0f, 1.0f);

    glm::vec4 world = glm::inverse(viewProj) * screen;
    glm::vec2 pointWorld = glm::vec2(world.x, world.y);

    for(const auto& tri : trianglePlanes)
    {
        if(tri.isClicked(pointWorld.x, pointWorld.y));
            return true;
    }
    return false;
}