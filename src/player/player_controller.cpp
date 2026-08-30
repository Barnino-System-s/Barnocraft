#include "player/player_controller.hpp"
#include "world/world.hpp"
#include "blocks/block_type.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace Barnocraft::Player {

    PlayerController::PlayerController() 
        : camera(glm::vec3(0.0f, 1.6f, 0.0f)),
          position(0, 10, 0),
          moveSpeed(5.0f),
          mouseSensitivity(0.1f),
          reachDistance(5.0f),
          firstMouse(true),
          lastMouseX(0.0),
          lastMouseY(0.0) {}

    PlayerController::~PlayerController() {}

    void PlayerController::initialize(GLFWwindow* window) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowUserPointer(window, this);
        
        // Set callbacks
        glfwSetCursorPosCallback(window, [](GLFWwindow* w, double xpos, double ypos) {
            PlayerController* self = static_cast<PlayerController*>(glfwGetWindowUserPointer(w));
            if (self) self->processMouseMovement(xpos, ypos);
        });
    }

    void PlayerController::processInput(GLFWwindow* window, World::World& world, float deltaTime) {
        // Movement
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        float speed = moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed *= 1.5f;
        }

        // WASD movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.position += camera.front * speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.position -= camera.front * speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.position -= camera.right * speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.position += camera.right * speed * deltaTime;
        }

        // Jump (space)
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.position.y += 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            camera.position.y -= 0.1f;
        }

        // Block breaking (left mouse button)
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            tryBreakBlock(world);
        }

        // Block placing (right mouse button)
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            tryPlaceBlock(world);
        }

        // Update world with camera position
        position = World::Vector3i(
            static_cast<int>(camera.position.x),
            static_cast<int>(camera.position.y),
            static_cast<int>(camera.position.z)
        );
    }

    void PlayerController::processMouseMovement(double xpos, double ypos) {
        if (firstMouse) {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;
        }

        double xoffset = xpos - lastMouseX;
        double yoffset = lastMouseY - ypos;
        lastMouseX = xpos;
        lastMouseY = ypos;

        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        camera.yaw += xoffset;
        camera.pitch += yoffset;

        // Clamp pitch
        if (camera.pitch > 89.0f) camera.pitch = 89.0f;
        if (camera.pitch < -89.0f) camera.pitch = -89.0f;

        camera.updateVectors();
    }

    void PlayerController::processMouseScroll(double xoffset, double yoffset) {
        camera.fov -= yoffset;
        if (camera.fov < 1.0f) camera.fov = 1.0f;
        if (camera.fov > 45.0f) camera.fov = 45.0f;
    }

    void PlayerController::tryBreakBlock(World::World& world) {
        // Simple raycast in the direction the camera is facing
        // For simplicity, we'll just break the block in front of the camera
        glm::vec3 rayStart = camera.position;
        glm::vec3 rayEnd = camera.position + camera.front * reachDistance;

        // Convert to integer coordinates
        World::Vector3i blockPos(
            static_cast<int>(rayEnd.x),
            static_cast<int>(rayEnd.y),
            static_cast<int>(rayEnd.z)
        );

        world.destroyBlock(blockPos);
    }

    void PlayerController::tryPlaceBlock(World::World& world) {
        // Simple placement - place block in front of camera
        glm::vec3 rayStart = camera.position;
        glm::vec3 rayEnd = camera.position + camera.front * reachDistance;

        World::Vector3i blockPos(
            static_cast<int>(rayEnd.x),
            static_cast<int>(rayEnd.y),
            static_cast<int>(rayEnd.z)
        );

        world.setBlock(blockPos, Blocks::BlockType::DIRT);
    }

} // namespace Barnocraft::Player
