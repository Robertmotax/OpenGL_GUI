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
#include <string>

class LightSource;
class RenderableObjectBase {
public:
    RenderableObjectBase(const std::vector<Tri>& triangles, Shader* shader);
    virtual ~RenderableObjectBase();
    std::vector<Vertex> getFlattenedVertices() { return flattenedVertices; }

    virtual void draw(const glm::mat4& viewProj, const std::vector<LightSource*>& lights, glm::vec3 cameraPos = glm::vec3(0.0f)) const = 0;
    virtual bool isClicked(float mouseX, float mouseY, int winWidth, int winHeight, const glm::mat4& viewProjInverse, float& outDistance) = 0;
    // Draggable by the mouse click handler and move it according to the mouse position
    virtual bool isDraggable() const { return false; }

    //Texture purposes 
    void setTexture(Texture* texture);
    void enableTexture(bool enable);
    Texture* texture = nullptr;

    //Enable clicking on the object
    void setOnClick(std::function<void()> callback);
    std::function<void()> onClick = {};

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 getModelMatrix() const {
        return model;
    }

    glm::mat4 getLocalMatrix() const {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, position);
        m = glm::rotate(m, rotation.x, glm::vec3(1, 0, 0));
        m = glm::rotate(m, rotation.y, glm::vec3(0, 1, 0));
        m = glm::rotate(m, rotation.z, glm::vec3(0, 0, 1));
        m = glm::scale(m, scale);
        return m;
    }

    //getters and setters
    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getPosition() const { return position; }
    void setName(const std::string objName) { name = objName; }
    std::string getName() const { return name; }

    // Get/Set the model matrix for transformations
    void setModelMatrix(const glm::mat4& mat) { model = mat; }
    std::vector<Tri> tris;
    int id;

    
protected:
    void computeBounds();

    std::string name;
    static int lastId;
    GLuint vao = 0, vbo = 0;
    int vertexCount = 0;
    Shader* shader = nullptr;
    bool useTexture = false; // controlled from UI or logic
    std::vector<Vertex> flattenedVertices;  // needed for OpenGL and bounds
    glm::vec3 minBounds, maxBounds;
    glm::mat4 model = glm::mat4(1.0f); // Model matrix for transformations
};
