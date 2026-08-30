#pragma once

#include "world/chunk.hpp"
#include "world/world.hpp"
#include <vector>
#include <memory>

namespace Barnocraft::Rendering {

    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };

    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    class ChunkMeshBuilder {
    public:
        ChunkMeshBuilder();
        ~ChunkMeshBuilder();

        MeshData buildMesh(const World::Chunk& chunk, World::World& world);

    private:
        bool shouldRenderFace(const World::Chunk& chunk, World::World& world, 
                             int x, int y, int z, int offsetX, int offsetY, int offsetZ) const;
        
        void addFrontFace(MeshData& mesh, float x, float y, float z, float u, float v);
        void addBackFace(MeshData& mesh, float x, float y, float z, float u, float v);
        void addLeftFace(MeshData& mesh, float x, float y, float z, float u, float v);
        void addRightFace(MeshData& mesh, float x, float y, float z, float u, float v);
        void addTopFace(MeshData& mesh, float x, float y, float z, float u, float v);
        void addBottomFace(MeshData& mesh, float x, float y, float z, float u, float v);

        float getBlockUV(Blocks::BlockType type, int face) const;
    };

} // namespace Barnocraft::Rendering
