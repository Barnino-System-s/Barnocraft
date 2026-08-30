#pragma once

#include "player/camera.hpp"
#include "world/vector3i.hpp"
#include <GLFW/glfw3.h>

namespace Barnocraft::World {
    class World;
}

namespace Barnocraft::Player {

    class PlayerController {
    public:
        Camera camera;
        World::Vector3i position;
        
        float moveSpeed;
        float mouseSensitivity;
        float reachDistance;
        
        bool firstMouse;
        double lastMouseX;
        double lastMouseY;

        PlayerController();
        ~PlayerController();

        void initialize(GLFWwindow* window);
        void processInput(GLFWwindow* window, World::World& world, float deltaTime);
        void processMouseMovement(double xpos, double ypos);
        void processMouseScroll(double xoffset, double yoffset);

    private:
        void tryBreakBlock(World::World& world);
        void tryPlaceBlock(World::World& world);
    };

} // namespace Barnocraft::Player
