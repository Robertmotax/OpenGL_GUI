#pragma once

#include <GL\glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera {
    public:

        Camera(float aspectRatio);
        ~Camera();
        
        void updateViewMatrix();
        void updateCameraVectors();
        void updateProjectionMatrix(float aspectRatio);
        void updateKeyControl(float deltaTime, GLFWwindow* window);
        glm::mat4 getViewProjection() const { return projection * view; }
        glm::mat4 getInverseViewProjection() const { return glm::inverse(projection * view); }
        glm::vec3 getPosition() const { return position; }
        glm::vec3 getCameraFront() const { return cameraFront; }

        
    private:
        glm::vec3 position;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
        glm::vec3 cameraRight;
        glm::vec3 worldUp;

        float yaw = -90.0f;
        float pitch = 0.0f;
        float fov = 45.0f;  // Add this line: field of view in degrees

        float movementSpeed = 2.50f; // Adjust as you want
        float rotationSpeed = 90.0f; // degrees per second

        glm::mat4 view;
        glm::mat4 projection;

        // Helper function to clamp angles
        // This ensures that yaw and pitch stay within reasonable limits
        void clampAngles();

        bool isSpeedBoosted = false;
};
