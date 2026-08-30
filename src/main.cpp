#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "world/world.hpp"
#include "world/world_constants.hpp"
#include "rendering/opengl_renderer.hpp"
#include "player/player_controller.hpp"

using namespace Barnocraft;

// Window dimensions
constexpr unsigned int SCR_WIDTH = 1280;
constexpr unsigned int SCR_HEIGHT = 720;

// Global variables
GLFWwindow* window = nullptr;
World::World* world = nullptr;
Rendering::OpenGLRenderer* renderer = nullptr;
Player::PlayerController* player = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (player) {
        player->processMouseMovement(xpos, ypos);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (player) {
        player->processMouseScroll(xoffset, yoffset);
    }
}

void processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (player && world) {
        player->processInput(window, *world, deltaTime);
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create window
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Barnocraft", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize world
    world = new World::World("BarnocraftSeed");
    
    // Initialize renderer
    renderer = new Rendering::OpenGLRenderer();
    if (!renderer->initialize(window)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        delete world;
        glfwTerminate();
        return -1;
    }

    // Initialize player
    player = new Player::PlayerController();
    player->initialize(window);

    // Set up projection matrix
    glm::mat4 projection = glm::perspective(
        glm::radians(player->camera.fov),
        static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
        0.1f, 1000.0f
    );
    renderer->setProjectionMatrix(glm::value_ptr(projection));

    // Connect block changed callback
    world->setOnBlockChangedCallback([](const World::Vector3i& pos, 
                                         Blocks::BlockType oldType, Blocks::BlockType newType) {
        if (renderer) {
            renderer->onBlockChanged(pos, oldType, newType);
        }
    });

    // Main render loop
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window, deltaTime);

        // Update world
        if (world && player) {
            world->update(player->position);
        }

        // Render
        glClearColor(0.53f, 0.81f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view matrix
        glm::mat4 view = player->camera.getViewMatrix();
        renderer->setViewMatrix(glm::value_ptr(view));

        // Render world
        if (world) {
            renderer->render(*world);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    if (player) delete player;
    if (renderer) delete renderer;
    if (world) delete world;

    glfwTerminate();
    return 0;
}
