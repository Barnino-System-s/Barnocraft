#include "rendering/opengl_renderer.hpp"
#include "rendering/chunk_mesh_builder.hpp"
#include "rendering/shader.hpp"
#include "world/world.hpp"
#include "world/chunk.hpp"
#include "blocks/block_type.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace Barnocraft::Rendering {

    OpenGLRenderer::OpenGLRenderer() {}
    
    OpenGLRenderer::~OpenGLRenderer() {
        cleanup();
    }

    bool OpenGLRenderer::initialize(GLFWwindow* p_window) {
        window = p_window;
        
        // Initialize GLEW
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            return false;
        }

        // Create shader program
        shaderProgram = createShaderProgram();
        if (shaderProgram == 0) {
            std::cerr << "Failed to create shader program" << std::endl;
            return false;
        }

        // Create texture
        texture = createTexture();
        if (texture == 0) {
            std::cerr << "Failed to create texture" << std::endl;
            return false;
        }

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        return true;
    }

    void OpenGLRenderer::cleanup() {
        // Cleanup chunk meshes
        for (auto& pair : chunkMeshes) {
            if (pair.second.vao) glDeleteVertexArrays(1, &pair.second.vao);
            if (pair.second.vbo) glDeleteBuffers(1, &pair.second.vbo);
            if (pair.second.ebo) glDeleteBuffers(1, &pair.second.ebo);
        }
        chunkMeshes.clear();

        if (texture) glDeleteTextures(1, &texture);
        if (shaderProgram) glDeleteProgram(shaderProgram);
        
        texture = 0;
        shaderProgram = 0;
    }

    GLuint OpenGLRenderer::compileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Shader compilation failed: " << infoLog << std::endl;
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    GLuint OpenGLRenderer::createShaderProgram() {
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE.c_str());
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE.c_str());

        if (vertexShader == 0 || fragmentShader == 0) {
            return 0;
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "Shader program linking failed: " << infoLog << std::endl;
            return 0;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return program;
    }

    GLuint OpenGLRenderer::createTexture() {
        // Create a simple white texture for now
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        
        // Create a simple checkerboard texture
        unsigned char data[4] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        return tex;
    }

    void OpenGLRenderer::createChunkMesh(ChunkMesh& mesh, const MeshData& data) {
        glGenVertexArrays(1, &mesh.vao);
        glGenBuffers(1, &mesh.vbo);
        glGenBuffers(1, &mesh.ebo);

        glBindVertexArray(mesh.vao);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), 
                     data.vertices.data(), GL_STATIC_DRAW);

        // Element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(uint32_t),
                     data.indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                              (void*)offsetof(Vertex, x));

        // Normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, nx));

        // Texture coordinate attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, u));

        glBindVertexArray(0);

        mesh.vertexCount = data.vertices.size();
        mesh.indexCount = data.indices.size();
        mesh.needsUpdate = false;
    }

    void OpenGLRenderer::updateChunkMesh(ChunkMesh& mesh, const MeshData& data) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), 
                     data.vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(uint32_t),
                     data.indices.data(), GL_STATIC_DRAW);

        mesh.vertexCount = data.vertices.size();
        mesh.indexCount = data.indices.size();
        mesh.needsUpdate = false;
    }

    void OpenGLRenderer::setupChunkMesh(World::Chunk* chunk) {
        size_t hash = chunk->coordinates.hash();
        
        if (chunkMeshes.find(hash) == chunkMeshes.end()) {
            // Create new mesh
            MeshData meshData = meshBuilder.buildMesh(*chunk, *chunk->world);
            ChunkMesh mesh;
            createChunkMesh(mesh, meshData);
            chunkMeshes[hash] = mesh;
        } else {
            // Update existing mesh
            if (chunk->needsMeshRebuild) {
                MeshData meshData = meshBuilder.buildMesh(*chunk, *chunk->world);
                updateChunkMesh(chunkMeshes[hash], meshData);
                chunk->markMeshAsRebuilt();
            }
        }
    }

    void OpenGLRenderer::renderChunkMesh(const ChunkMesh& mesh, const World::ChunkCoordinates& coord) {
        if (mesh.vao == 0 || mesh.indexCount == 0) return;

        // Calculate model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(
            coord.x * World::CHUNK_SIZE_X,
            0.0f,
            coord.z * World::CHUNK_SIZE_Z
        ));

        // Use shader program
        glUseProgram(shaderProgram);

        // Set uniforms
        GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Draw
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indexCount), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void OpenGLRenderer::render(World::World& world) {
        // Clear screen
        glClearColor(0.53f, 0.81f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update chunks that need rebuilding
        updateChunks(world);

        // Render all loaded chunks
        for (World::Chunk* chunk : world.getChunkManager()->getLoadedChunks()) {
            auto it = chunkMeshes.find(chunk->coordinates.hash());
            if (it != chunkMeshes.end()) {
                renderChunkMesh(it->second, chunk->coordinates);
            }
        }
    }

    void OpenGLRenderer::updateChunks(World::World& world) {
        for (World::Chunk* chunk : world.getChunkManager()->getLoadedChunks()) {
            if (chunk->needsMeshRebuild) {
                setupChunkMesh(chunk);
            }
        }
    }

    void OpenGLRenderer::onBlockChanged(const World::Vector3i& worldPos, 
                                       Blocks::BlockType oldType, Blocks::BlockType newType) {
        // Mark the chunk containing this block for rebuild
        World::ChunkCoordinates chunkCoord = World::ChunkCoordinates::from_world_position_int(
            worldPos.x, worldPos.z);
        
        auto it = chunkMeshes.find(chunkCoord.hash());
        if (it != chunkMeshes.end()) {
            it->second.needsUpdate = true;
        }
    }

    void OpenGLRenderer::setViewMatrix(const float* viewMatrix) {
        glUseProgram(shaderProgram);
        GLint viewLoc = glGetUniformLocation(shaderProgram, "uView");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMatrix);
    }

    void OpenGLRenderer::setProjectionMatrix(const float* projectionMatrix) {
        glUseProgram(shaderProgram);
        GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projectionMatrix);
    }

} // namespace Barnocraft::Rendering
