#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Barnocraft::World {
    class World;
    class Chunk;
    struct ChunkCoordinates;
}

namespace Barnocraft::Rendering {

    struct Vertex;
    struct MeshData;

    class OpenGLRenderer {
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();

        bool initialize(GLFWwindow* window);
        void cleanup();

        void render(World::World& world);
        void updateChunks(World::World& world);

        void onBlockChanged(const World::Vector3i& worldPos, 
                           Blocks::BlockType oldType, Blocks::BlockType newType);

        void setViewMatrix(const float* viewMatrix);
        void setProjectionMatrix(const float* projectionMatrix);

    private:
        struct ChunkMesh {
            GLuint vao = 0;
            GLuint vbo = 0;
            GLuint ebo = 0;
            size_t vertexCount = 0;
            size_t indexCount = 0;
            bool needsUpdate = true;
        };

        GLFWwindow* window = nullptr;
        GLuint shaderProgram = 0;
        GLuint texture = 0;
        
        std::unordered_map<size_t, ChunkMesh> chunkMeshes;
        ChunkMeshBuilder meshBuilder;

        GLuint compileShader(GLenum type, const char* source);
        GLuint createShaderProgram();
        GLuint createTexture();
        
        void createChunkMesh(ChunkMesh& mesh, const MeshData& data);
        void updateChunkMesh(ChunkMesh& mesh, const MeshData& data);
        void renderChunkMesh(const ChunkMesh& mesh, const World::ChunkCoordinates& coord);
        
        void setupChunkMesh(World::Chunk* chunk);
    };

} // namespace Barnocraft::Rendering
