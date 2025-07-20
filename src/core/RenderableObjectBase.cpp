#include "core/RenderableObjectBase.h"
#include <iostream>
#include "core/Vertex.h"

RenderableObjectBase::RenderableObjectBase(const std::vector<Tri>& triangles, Shader* shader)
    : tris(triangles), shader(shader)
{
    // Flatten triangles into vertices
    for (const auto& tri : tris) {
        flattenedVertices.push_back(tri.v0);
        flattenedVertices.push_back(tri.v1);
        flattenedVertices.push_back(tri.v2);
    }

    vertexCount = static_cast<int>(flattenedVertices.size());

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexCount, flattenedVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    //Texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glBindAttribLocation(shader->getID(), 0, "position");
    glBindAttribLocation(shader->getID(), 1, "color");
}

RenderableObjectBase::~RenderableObjectBase() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void RenderableObjectBase::setTexture(Texture* texture) {
    this->texture = texture; // Store the texture for later use
}

void RenderableObjectBase::enableTexture(bool enable) {
    useTexture = enable && (texture != nullptr);
}

void RenderableObjectBase::setOnClick(std::function<void()> callback) {
    onClick = callback;
}
