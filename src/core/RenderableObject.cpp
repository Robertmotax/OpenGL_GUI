#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <singleton/RayPicker.h>
#include "core/RenderableObject.h"
#include "core/LightSource.h"
#include <algorithm>
#include <iostream>
#include "core/Vertex.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include "core/util.h"
#include "Globals.h"
#include <ui/Sidebar.h>

class LightSource;
RenderableObject::RenderableObject(const std::vector<Tri>& triangles, Shader* shader, Shader* shaderShadow, bool unlit)
    : RenderableObjectBase(triangles, shader)
{
    this->shaderShadow = shaderShadow;
    isUnlit = unlit;
    setOnClick([this]() {
        setSelectedObject(this);
        std::cout << "Selected obj is now " << this->getName();
    });
    updateLocalTransformFromComponents();
}

void RenderableObject::draw(const glm::mat4& viewProj, const std::vector<LightSource*>& lights, glm::vec3 cameraPos) const {
    shader->use();

    glm::mat4 model = getModelMatrix();
    GLuint shaderID = shader->getID();

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uVP"), 1, GL_FALSE, glm::value_ptr(viewProj));

    glUniform3fv(glGetUniformLocation(shaderID, "uViewPos"), 1, glm::value_ptr(cameraPos));
    glUniform1f(glGetUniformLocation(shaderID, "shininess"), 32.0f);
    glUniform1f(glGetUniformLocation(shaderID, "specularStrength"), 0.5f);

    // --- Texture 2D binding ---
    if (useTexture && texture) {
        glUniform1i(glGetUniformLocation(shaderID, "uUseTexture"), 1);
        texture->useTexture();
        glUniform1i(glGetUniformLocation(shaderID, "uTexture"), 0);
    } else {
        glUniform1i(glGetUniformLocation(shaderID, "uUseTexture"), 0);
    }
    glUniform1i(glGetUniformLocation(shaderID, "uIsUnlit"), isUnlit ? 1 : 0);
    
    // Start from stored position
    glm::mat4 modelTransformed = glm::translate(model, position);

    // --- Shadow cubemap lights ---
    if (!isUnlit)
    {
        glUniform1i(glGetUniformLocation(shaderID, "uNumLights"), (int)lights.size());

        for (int i = 0; i < (int)lights.size(); ++i) {
            const LightSource* light = lights[i];
            std::string idx = std::to_string(i);

            glUniform3fv(glGetUniformLocation(shaderID, ("lightPositions[" + idx + "]").c_str()), 1, glm::value_ptr(*light->position));
            glUniform3fv(glGetUniformLocation(shaderID, ("lightColors[" + idx + "]").c_str()), 1, glm::value_ptr(*light->color));
            glUniform1f(glGetUniformLocation(shaderID, ("lightIntensities[" + idx + "]").c_str()), glm::length(*light->intensity));
            glUniform1f(glGetUniformLocation(shaderID, ("farPlanes[" + idx + "]").c_str()), light->farPlane);

            glActiveTexture(GL_TEXTURE1 + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, light->getShadowCubemap());
            glUniform1i(glGetUniformLocation(shaderID, ("shadowMaps[" + idx + "]").c_str()), 1 + i);
        }
    } else {
        glUniform1i(glGetUniformLocation(shaderID, "uNumLights"), 0);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    // Optional: debug OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}


void RenderableObject::drawDepthOnly(const glm::mat4& shadowMatrix, const glm::vec3& lightPos, float farPlane) const {
    if (!shaderShadow) {
        std::cerr << "shaderShadow is null!\n";
        return;
    }
    //std::cout << glm::to_string(shadowMatrix) << std::endl;

    shaderShadow->use();

    //std::cout << "Setting uShadowMatrix uniform\n";
    shaderShadow->setMat4("uShadowMatrix", shadowMatrix);

    //std::cout << "Setting uFarPlane uniform\n";
    shaderShadow->setFloat("uFarPlane", farPlane);

    //std::cout << "Setting uLightPos uniform\n";
    shaderShadow->setVec3("uLightPos", lightPos);

    //std::cout << "Setting uModel uniform\n";
    glm::mat4 modelMatrix = getModelMatrix();
    shaderShadow->setMat4("uModel", modelMatrix);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}


bool RenderableObject::isClicked(float mouseX, float mouseY, int winWidth, int winHeight, const glm::mat4& viewProjInverse, float& outDistance) {
    //get origin (camera position) and object's position (direction)
    glm::vec3 rayOrigin, rayDir;
    RayPicker::getInstance().screenPosToWorldRay(mouseX, mouseY, winWidth, winHeight, rayOrigin, rayDir);
    
    return isRayIntersecting(rayOrigin, rayDir, outDistance);
}


/**
 * @brief Checks if a ray intersects this renderable object's mesh (per-triangle intersection).
 * 
 * This method transforms each triangle into world space using the object's model matrix
 * and performs an accurate ray-triangle intersection test (Möller–Trumbore algorithm).
 * 
 * @author Marcus Sharma
 * @param rayOrigin     The origin point of the ray in world space.
 * @param rayDirection  The normalized direction of the ray in world space.
 * @param outDistance   If hit, will store the distance to the closest intersected triangle.
 * @return true if the ray intersects any triangle of the object, false otherwise.
 */
bool RenderableObject::isRayIntersecting(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& outDistance) {
    glm::mat4 modelMatrix = getModelMatrix();
    //3: Accurate per-triangle intersection (narrow-phase)
    float closestT = FLT_MAX;
    bool hit = false;

    for (const Tri& tri : tris) {
        glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(tri.v0.position, 1.0f));
        glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(tri.v1.position, 1.0f));
        glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(tri.v2.position, 1.0f));

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 h = glm::cross(rayDirection, edge2);
        float a = glm::dot(edge1, h);
        if (fabs(a) < 1e-6f) continue;

        float f = 1.0f / a;
        glm::vec3 s = rayOrigin - v0;
        float u = f * glm::dot(s, h);
        if (u < 0.0f || u > 1.0f) continue;

        glm::vec3 q = glm::cross(s, edge1);
        float v = f * glm::dot(rayDirection, q);
        if (v < 0.0f || u + v > 1.0f) continue;

        float t = f * glm::dot(edge2, q);
        if (t > 0.001f && t < closestT) {
            closestT = t;
            hit = true;
        }
    }

    if (hit) {
        outDistance = closestT;
    }

    return hit;
}


void RenderableObject::setParent(RenderableObject* newParent) {
    // Step 1: Cache the current world transform before parenting
    glm::mat4 worldTransform = model;
    if(!newParent)
    {
        parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
        parent = nullptr;
        localTransform = worldTransform;
        return;
    }

    // Step 2: Remove from current parent's children list (if any)
    if (parent) {
        auto& siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    // Step 3: Assign new parent
    parent = newParent;

    // Step 4: Adjust localTransform so world position remains the same
    if (newParent) {
        glm::mat4 parentWorld = newParent->model; // current world transform
        localTransform = glm::inverse(parentWorld) * worldTransform;

        // Step 5: Add this to the new parent's children list
        parent->children.push_back(this);
    } else {
        // If unparented, keep world transform as local
        localTransform = worldTransform;
    }
}



void RenderableObject::updateSelfAndChildren() {
    // Compute world transform
    if (parent) {
        model = parent->getModelMatrix() * localTransform;  // World = ParentWorld * Local
    } else {
        model = localTransform;
    }

    // Update children recursively
    for (RenderableObject* child : children) {
        child->updateSelfAndChildren();
    }
}


void RenderableObject::deleteObject()
{
    detachFromParent();
    cleanupRemainingData();
}

void RenderableObject::detachFromParent() 
{
    for (RenderableObject* child : children) {
        child->parent = nullptr;                 // Make the children object root-level objects
        child->model = child->localTransform;    // Reset to local transform
    }
    children.clear(); // Remove all links from this object

    if (parent) {
        auto& siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        parent = nullptr;
    }
}

void RenderableObject::cleanupRemainingData() 
{
    // Unbind before deletion
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;  // Avoid dangling
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if (texture) {
        delete texture;
        texture = nullptr;
    }
}

void RenderableObject::updateFromKeyframes(float currentTime) {
    if (keyframes.size() < 2) return;  // Need at least two keyframes to interpolate

    Keyframe* prev = nullptr;
    Keyframe* next = nullptr;

    // Find the two keyframes surrounding currentTime
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        if (keyframes[i].time <= currentTime && keyframes[i + 1].time >= currentTime) {
            prev = &keyframes[i];
            next = &keyframes[i + 1];
            break;
        }
    }

    if (!prev || !next) {
        // Outside the keyframe time range:
        // Snap to first or last keyframe transform
        if (currentTime < keyframes.front().time) {
            const auto& k = keyframes.front();
            localTransform = glm::translate(glm::mat4(1.0f), k.position) *
                             glm::mat4_cast(glm::quat(k.rotation)) *
                             glm::scale(glm::mat4(1.0f), k.scale);
        } else if (currentTime > keyframes.back().time) {
            const auto& k = keyframes.back();
            localTransform = glm::translate(glm::mat4(1.0f), k.position) *
                             glm::mat4_cast(glm::quat(k.rotation)) *
                             glm::scale(glm::mat4(1.0f), k.scale);
        }
        return;
    }

    // Interpolation factor (0 to 1)
    float t = (currentTime - prev->time) / (next->time - prev->time);

    // Linear interpolate position and scale
    glm::vec3 interpPos = glm::mix(prev->position, next->position, t);
    glm::vec3 interpScale = glm::mix(prev->scale, next->scale, t);

    glm::quat rotPrev = glm::quat(prev->rotation);
    glm::quat rotNext = glm::quat(next->rotation);

    // Slerp rotation
    glm::quat interpRot = glm::slerp(rotPrev, rotNext, t);

    // Compose local transform matrix from position, rotation, scale
    localTransform = glm::translate(glm::mat4(1.0f), interpPos) *
                     glm::mat4_cast(interpRot) *
                     glm::scale(glm::mat4(1.0f), interpScale);
}
