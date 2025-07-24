// RenderableObjectBase.h
#pragma once

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "core/Shader.h"
#include "core/Vertex.h"
#include "core/Tri.h"
#include "core/Texture.h"
#include "core/LightSource.h"

class RenderableObjectBase {
public:
    RenderableObjectBase(const std::vector<Tri>& triangles, Shader* shader);
    virtual ~RenderableObjectBase();

    virtual void draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const = 0;
    virtual bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, glm::mat4 viewProjInverse) = 0;
    //Texture purposes 
    void setTexture(Texture* texture);
    void enableTexture(bool enable);

    //Enable clicking on the object
    void setOnClick(std::function<void()> callback);
    std::function<void()> onClick;

    //getters and setters
    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getPosition() const { return position; }

protected:
    void computeBounds();

    GLuint vao = 0, vbo = 0;
    int vertexCount = 0;
    Shader* shader = nullptr;
    std::vector<Tri> tris;
    Texture* texture = nullptr; // Optional texture for the object
    bool useTexture = false; // controlled from UI or logic
    std::vector<Vertex> flattenedVertices;  // needed for OpenGL and bounds
    glm::vec3 minBounds, maxBounds;

    glm::vec3 position=  glm::vec3(0.0f);  // Default origin position; // position of the object based on the xyz- opengl coordinates
};
