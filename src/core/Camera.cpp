// Camera.cpp
#include <glm/glm.hpp>
#include "core/Camera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
static const glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

Camera::Camera(float aspectRatio)
    : position(0.0f, 0.0f, 3.0f),   // Camera positioned back to see scene
      fov(45.0f)                    // Initialize FOV here
{
    worldUp = WORLD_UP;
    yaw = -90.0f;
    pitch = 0.0f;
    movementSpeed = 2.5f;      // Adjust as you want
    rotationSpeed = 90.0f; // degrees per second

    updateViewMatrix();
    updateProjectionMatrix(aspectRatio);
}

void Camera::updateProjectionMatrix(float aspectRatio) {
    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

// Helper function to create a rotation matrix from yaw, pitch, and roll
glm::mat4 yawPitchRoll(float yaw, float pitch, float roll) {
    glm::mat4 rot(1.0f);
    rot = glm::rotate(rot, yaw, glm::vec3(0,1,0));
    rot = glm::rotate(rot, pitch, glm::vec3(1,0,0));
    rot = glm::rotate(rot, roll, glm::vec3(0,0,1));
    return rot;
}

//
void Camera::updateViewMatrix() {
    //recalculates the camera's direction vectors based on yaw and pitch
    updateCameraVectors();

    //glm::lookAt is composed of eye, center and up vectors
    // - eye position is camera position
    // - center position is where the camera is looking (position + front vector)
    // - up vector defines camera's upward direction
    view = glm::lookAt(position, position + cameraFront, cameraUp);
}

void Camera::updateCameraVectors() {
    // glm::mat4 rotation = yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
    // glm::vec3 forward = glm::vec3(rotation * glm::vec4(0, 0, -1, 0));
    // glm::vec3 up = glm::vec3(rotation * glm::vec4(0, 1, 0, 0));
    // view = glm::lookAt(position, position + forward, up);

    // Calculate front vector from yaw and pitch angles
    glm::vec3 front;
    front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    front.y = sinf(glm::radians(pitch));
    front.z = cosf(glm::radians(pitch)) * sinf(glm::radians(yaw));
    cameraFront = glm::normalize(front);

    // Recalculate right and up vectors from camera front and world up
    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void Camera::updateKeyControl(float deltaTime, GLFWwindow* window) {
    glm::vec3 moveDir(0.0f);

    // WASD for X/Y movement
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= cameraFront;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= cameraRight;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += cameraRight;
    // Z movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) moveDir += worldUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) moveDir -= worldUp;
    // Rotation
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) yaw -= rotationSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) yaw += rotationSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) pitch += rotationSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) pitch -= rotationSpeed * deltaTime;

    if (glm::length(moveDir) > 0.0f)
        moveDir = glm::normalize(moveDir);

    // glm::mat4 rotation = yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
    // glm::vec3 worldDir = glm::vec3(rotation * glm::vec4(moveDir, 0.0f));
    //position += worldDir * movementSpeed * deltaTime;

    position += moveDir * movementSpeed * deltaTime;
    // Clamp angles to prevent gimbal lock
    clampAngles();

    updateViewMatrix();
}

// Clamp pitch to prevent flip --this will prevent future issues when dealing with texture mapping
void Camera::clampAngles() {
    //Vertical angles should be clamped to prevent flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    //Horizontal angles can wrap around
    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < -360.0f) yaw += 360.0f;
}
Camera::~Camera() 
{}