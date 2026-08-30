cmake_minimum_required(VERSION 3.16)
project(Barnocraft VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

# Download and include GLM (header-only)
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.0
)
FetchContent_MakeAvailable(glm)

# Include directories
include_directories(
    include
    ${OPENGL_INCLUDE_DIRS}
    ${glfw3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${glm_SOURCE_DIR}
)

# Source files
set(SOURCES
    src/main.cpp
    
    # Blocks
    src/blocks/block_registry.cpp
    
    # World
    src/world/chunk.cpp
    src/world/world.cpp
    src/world/chunk_manager.cpp
    
    # Generation
    src/generation/chunk_generator.cpp
    
    # Storage
    src/storage/world_storage.cpp
    
    # Rendering
    src/rendering/chunk_mesh_builder.cpp
    src/rendering/opengl_renderer.cpp
    
    # Player
    src/player/camera.cpp
    src/player/player_controller.cpp
)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    GLEW::GLEW
    ${CMAKE_DL_LIBS}
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 gdi32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} X11)
endif()

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
