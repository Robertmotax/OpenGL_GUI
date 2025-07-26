#include "core/RenderableObjectBase.h"
#include <iostream>
#include "core/Vertex.h"
int RenderableObjectBase::lastId = 0;

RenderableObjectBase::RenderableObjectBase(const std::vector<Tri>& triangles, Shader* shader)
    : tris(triangles), shader(shader)
{
    id = ++lastId;
    name = "obj" + id;
    // Flatten triangles into vertices
    for (const auto &tri : tris)
    {
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

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

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