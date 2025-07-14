#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera {
    public:
        Camera(float aspectRatio);

        void updateViewMatrix();
        void updateProjectionMatrix(float aspectRatio);
        void update(float deltaTime, GLFWwindow* window);

        glm::mat4 getViewProjection() const { return projection * view; }
        glm::mat4 getInverseViewProjection() const { return glm::inverse(projection * view); }

    private:
        glm::vec3 position;
        float yaw = 0.0f;
        float pitch = 0.0f;

        float fov = 45.0f;  // Add this line: field of view in degrees

        float moveSpeed = 2.5f;      // Adjust as you want
        float rotationSpeed = 90.0f; // degrees per second

        glm::mat4 view;
        glm::mat4 projection;
};
