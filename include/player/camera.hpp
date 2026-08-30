#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Barnocraft::Player {

    class Camera {
    public:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;
        
        float yaw;
        float pitch;
        float fov;

        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
               float yaw = -90.0f, float pitch = 0.0f);

        glm::mat4 getViewMatrix() const;
        void updateVectors();
    };

} // namespace Barnocraft::Player
