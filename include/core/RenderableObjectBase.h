// RenderableObjectBase.h
#pragma once

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "core/Shader.h"
#include "core/Vertex.h"
#include "core/Tri.h"

class RenderableObjectBase {
public:
    RenderableObjectBase(const std::vector<Tri>& triangles, Shader* shader);
    virtual ~RenderableObjectBase();

    virtual void draw(const glm::mat4& viewProj) const = 0;
    virtual bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, glm::mat4 viewProjInverse) = 0;
    void setOnClick(std::function<void()> callback);
    std::function<void()> onClick;

protected:
    void computeBounds();

    GLuint vao = 0, vbo = 0;
    int vertexCount = 0;
    Shader* shader = nullptr;
    std::vector<Tri> trianglePlanes;
    std::vector<Vertex> flattenedVertices;  // needed for OpenGL and bounds
    glm::vec3 minBounds, maxBounds;
};
