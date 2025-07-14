// Camera.cpp
#include <glm/glm.hpp>
#include "core/Camera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(float aspectRatio)
    : position(0.0f, 0.0f, 3.0f),   // Camera positioned back to see scene
      fov(45.0f)                    // Initialize FOV here
{
    updateViewMatrix();
    updateProjectionMatrix(aspectRatio);
}

void Camera::updateProjectionMatrix(float aspectRatio) {
    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

glm::mat4 yawPitchRoll(float yaw, float pitch, float roll) {
    glm::mat4 rot(1.0f);
    rot = glm::rotate(rot, yaw, glm::vec3(0,1,0));
    rot = glm::rotate(rot, pitch, glm::vec3(1,0,0));
    rot = glm::rotate(rot, roll, glm::vec3(0,0,1));
    return rot;
}

void Camera::updateViewMatrix() {
    glm::mat4 rotation = yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
    glm::vec3 forward = glm::vec3(rotation * glm::vec4(0, 0, -1, 0));
    glm::vec3 up = glm::vec3(rotation * glm::vec4(0, 1, 0, 0));
    view = glm::lookAt(position, position + forward, up);
}

void Camera::update(float deltaTime, GLFWwindow* window) {
    glm::vec3 moveDir(0.0f);

    // WASD for X/Y movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir.z += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir.z -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir.x += 1.0f;

    // Z movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) moveDir.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) moveDir.y -= 1.0f;

    // Rotation
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) yaw -= rotationSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) yaw += rotationSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) pitch += rotationSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) pitch -= rotationSpeed * deltaTime;

    if (glm::length(moveDir) > 0.0f)
        moveDir = glm::normalize(moveDir);

    glm::mat4 rotation = yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
    glm::vec3 worldDir = glm::vec3(rotation * glm::vec4(moveDir, 0.0f));

    position += worldDir * moveSpeed * deltaTime;

    updateViewMatrix();
}

