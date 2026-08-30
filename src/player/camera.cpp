#include "player/camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Barnocraft::Player {

    Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
        : position(pos), worldUp(up), yaw(yaw), pitch(pitch), fov(45.0f) {
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        updateVectors();
    }

    glm::mat4 Camera::getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    void Camera::updateVectors() {
        // Calculate the new front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
        
        // Re-calculate the right and up vector
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

} // namespace Barnocraft::Player
