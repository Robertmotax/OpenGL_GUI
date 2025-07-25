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
    // Draggable by the mouse click handler and move it according to the mouse position
    virtual bool isDraggable() const { return false; }

    //Texture purposes 
    void setTexture(Texture* texture);
    void enableTexture(bool enable);

    //Enable clicking on the object
    void setOnClick(std::function<void()> callback);
    std::function<void()> onClick = {};

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);

        // Translate
        model = glm::translate(model, position);

        // Rotate (apply in order: X, Y, Z)
        model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
        model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
        model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));

        // Scale
        model = glm::scale(model, scale);

        return model;
    }

    //getters and setters
    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getPosition() const { return position; }

    // Set the model matrix for transformations
    void setModelMatrix(const glm::mat4& mat) { model = mat; }

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
    glm::mat4 model = glm::mat4(1.0f); // Model matrix for transformations
};
