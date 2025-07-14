#include "core/RenderableObject.h"
#include <iostream>
#include "core/Vertex.h"
#include <glm/gtc/type_ptr.hpp>

RenderableObject::RenderableObject(const std::vector<Tri>& triangles, Shader* shader)
    : RenderableObjectBase(triangles, shader)
{
    // No additional initialization needed.
}

void RenderableObject::draw(const glm::mat4& viewProj) const {
    shader->use();
    
    // Get the location of the uniform variable in the shader program
    GLuint location = glGetUniformLocation(shader->getID(), "uVP");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewProj));

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

bool RenderableObject::isClicked(float mouseX, float mouseY, int winWidth, int winHeight, glm::mat4 viewProjInverse) {
    // Convert mouse coords to normalized device coordinates (NDC)
    glm::vec4 screen = glm::vec4(
        mouseX / winWidth * 2.0f - 1.0f,
        1.0f - mouseY / winHeight * 2.0f,
        0.0f, 1.0f);

    // Transform from NDC back to world coordinates using the inverse viewProj matrix
    glm::vec4 world = viewProjInverse * screen;
    
    // Perspective divide if needed (usually w != 1 after transform)
    if (world.w != 0.0f) {
        world /= world.w;
    }

    glm::vec2 pointWorld = glm::vec2(world.x, world.y);

    for (const auto& tri : trianglePlanes) {
        if (tri.isClicked(pointWorld.x, pointWorld.y)) {
            return true;
        }
    }
    return false;
}
