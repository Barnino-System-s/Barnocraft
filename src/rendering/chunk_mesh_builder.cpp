#include "rendering/chunk_mesh_builder.hpp"
#include "blocks/block_type.hpp"
#include "blocks/block_registry.hpp"
#include "world/world_constants.hpp"

namespace Barnocraft::Rendering {

    ChunkMeshBuilder::ChunkMeshBuilder() {}
    ChunkMeshBuilder::~ChunkMeshBuilder() {}

    MeshData ChunkMeshBuilder::buildMesh(const World::Chunk& chunk, World::World& world) {
        MeshData meshData;

        for (int x = 0; x < World::CHUNK_SIZE_X; x++) {
            for (int y = 0; y < World::CHUNK_SIZE_Y; y++) {
                for (int z = 0; z < World::CHUNK_SIZE_Z; z++) {
                    Blocks::BlockType blockType = chunk.getBlock(x, y, z);

                    if (blockType == Blocks::BlockType::AIR) {
                        continue;
                    }

                    float u = getBlockUV(blockType, 0);
                    float v = getBlockUV(blockType, 1);

                    // Check each face
                    if (shouldRenderFace(chunk, world, x, y, z, 0, 0, 1)) {
                        addFrontFace(meshData, x, y, z, u, v);
                    }
                    if (shouldRenderFace(chunk, world, x, y, z, 0, 0, -1)) {
                        addBackFace(meshData, x, y, z, u, v);
                    }
                    if (shouldRenderFace(chunk, world, x, y, z, -1, 0, 0)) {
                        addLeftFace(meshData, x, y, z, u, v);
                    }
                    if (shouldRenderFace(chunk, world, x, y, z, 1, 0, 0)) {
                        addRightFace(meshData, x, y, z, u, v);
                    }
                    if (shouldRenderFace(chunk, world, x, y, z, 0, 1, 0)) {
                        addTopFace(meshData, x, y, z, u, v);
                    }
                    if (shouldRenderFace(chunk, world, x, y, z, 0, -1, 0)) {
                        addBottomFace(meshData, x, y, z, u, v);
                    }
                }
            }
        }

        return meshData;
    }

    bool ChunkMeshBuilder::shouldRenderFace(const World::Chunk& chunk, World::World& world,
                                           int x, int y, int z, int offsetX, int offsetY, int offsetZ) const {
        int neighborX = x + offsetX;
        int neighborY = y + offsetY;
        int neighborZ = z + offsetZ;

        Blocks::BlockType neighborBlockType;

        // Check if neighbor is within this chunk
        if (neighborX >= 0 && neighborX < World::CHUNK_SIZE_X &&
            neighborY >= 0 && neighborY < World::CHUNK_SIZE_Y &&
            neighborZ >= 0 && neighborZ < World::CHUNK_SIZE_Z) {
            neighborBlockType = chunk.getBlock(neighborX, neighborY, neighborZ);
        } else {
            // Neighbor is in another chunk
            int chunkXOffset = 0;
            if (neighborX < 0) chunkXOffset = -1;
            else if (neighborX >= World::CHUNK_SIZE_X) chunkXOffset = 1;

            int chunkZOffset = 0;
            if (neighborZ < 0) chunkZOffset = -1;
            else if (neighborZ >= World::CHUNK_SIZE_Z) chunkZOffset = 1;

            World::ChunkCoordinates neighborChunkCoord(
                chunk.coordinates.x + chunkXOffset,
                chunk.coordinates.z + chunkZOffset
            );

            const World::Chunk* neighborChunk = world.getChunkManager()->getChunk(neighborChunkCoord);
            if (neighborChunk == nullptr) {
                return true; // Render face if no neighbor chunk
            }

            // Adjust coordinates for neighbor chunk
            int localNeighborX = neighborX;
            if (neighborX < 0) localNeighborX = World::CHUNK_SIZE_X - 1;
            else if (neighborX >= World::CHUNK_SIZE_X) localNeighborX = 0;

            int localNeighborZ = neighborZ;
            if (neighborZ < 0) localNeighborZ = World::CHUNK_SIZE_Z - 1;
            else if (neighborZ >= World::CHUNK_SIZE_Z) localNeighborZ = 0;

            neighborBlockType = neighborChunk->getBlock(localNeighborX, neighborY, localNeighborZ);
        }

        const Blocks::Block& neighborBlock = Blocks::BlockRegistry::getBlock(neighborBlockType);
        return !neighborBlock.isSolid || neighborBlock.isTransparent;
    }

    void ChunkMeshBuilder::addFrontFace(MeshData& mesh, float x, float y, float z, float u, float v) {
        uint32_t baseIndex = mesh.vertices.size();
        
        mesh.vertices.push_back({x + 0, y + 0, z + 1, 0, 0, 1, u, v + 0.1f});
        mesh.vertices.push_back({x + 1, y + 0, z + 1, 0, 0, 1, u + 0.1f, v + 0.1f});
        mesh.vertices.push_back({x + 1, y + 1, z + 1, 0, 0, 1, u + 0.1f, v});
        mesh.vertices.push_back({x + 0, y + 1, z + 1, 0, 0, 1, u, v});

        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 1);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 3);
    }

    void ChunkMeshBuilder::addBackFace(MeshData& mesh, float x, float y, float z, float u, float v) {
        uint32_t baseIndex = mesh.vertices.size();
        
        mesh.vertices.push_back({x + 1, y + 0, z + 0, 0, 0, -1, u, v + 0.1f});
        mesh.vertices.push_back({x + 0, y + 0, z + 0, 0, 0, -1, u + 0.1f, v + 0.1f});
        mesh.vertices.push_back({x + 0, y + 1, z + 0, 0, 0, -1, u + 0.1f, v});
        mesh.vertices.push_back({x + 1, y + 1, z + 0, 0, 0, -1, u, v});

        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 1);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 3);
    }

    void ChunkMeshBuilder::addLeftFace(MeshData& mesh, float x, float y, float z, float u, float v) {
        uint32_t baseIndex = mesh.vertices.size();
        
        mesh.vertices.push_back({x + 0, y + 0, z + 0, -1, 0, 0, u, v + 0.1f});
        mesh.vertices.push_back({x + 0, y + 0, z + 1, -1, 0, 0, u + 0.1f, v + 0.1f});
        mesh.vertices.push_back({x + 0, y + 1, z + 1, -1, 0, 0, u + 0.1f, v});
        mesh.vertices.push_back({x + 0, y + 1, z + 0, -1, 0, 0, u, v});

        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 1);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 3);
    }

    void ChunkMeshBuilder::addRightFace(MeshData& mesh, float x, float y, float z, float u, float v) {
        uint32_t baseIndex = mesh.vertices.size();
        
        mesh.vertices.push_back({x + 1, y + 0, z + 1, 1, 0, 0, u, v + 0.1f});
        mesh.vertices.push_back({x + 1, y + 0, z + 0, 1, 0, 0, u + 0.1f, v + 0.1f});
        mesh.vertices.push_back({x + 1, y + 1, z + 0, 1, 0, 0, u + 0.1f, v});
        mesh.vertices.push_back({x + 1, y + 1, z + 1, 1, 0, 0, u, v});

        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 1);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 3);
    }

    void ChunkMeshBuilder::addTopFace(MeshData& mesh, float x, float y, float z, float u, float v) {
        uint32_t baseIndex = mesh.vertices.size();
        
        mesh.vertices.push_back({x + 0, y + 1, z + 1, 0, 1, 0, u, v});
        mesh.vertices.push_back({x + 1, y + 1, z + 1, 0, 1, 0, u + 0.1f, v});
        mesh.vertices.push_back({x + 1, y + 1, z + 0, 0, 1, 0, u + 0.1f, v + 0.1f});
        mesh.vertices.push_back({x + 0, y + 1, z + 0, 0, 1, 0, u, v + 0.1f});

        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 1);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 3);
    }

    void ChunkMeshBuilder::addBottomFace(MeshData& mesh, float x, float y, float z, float u, float v) {
        uint32_t baseIndex = mesh.vertices.size();
        
        mesh.vertices.push_back({x + 0, y + 0, z + 0, 0, -1, 0, u, v + 0.1f});
        mesh.vertices.push_back({x + 1, y + 0, z + 0, 0, -1, 0, u + 0.1f, v + 0.1f});
        mesh.vertices.push_back({x + 1, y + 0, z + 1, 0, -1, 0, u + 0.1f, v});
        mesh.vertices.push_back({x + 0, y + 0, z + 1, 0, -1, 0, u, v});

        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 1);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 0);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 3);
    }

    float ChunkMeshBuilder::getBlockUV(Blocks::BlockType type, int face) const {
        switch (type) {
            case Blocks::BlockType::GRASS_BLOCK:
                return face == 0 ? 0.0f : (face == 1 ? 0.1f : 0.0f);
            case Blocks::BlockType::DIRT:
                return face == 0 ? 0.1f : (face == 1 ? 0.2f : 0.1f);
            case Blocks::BlockType::COBBLESTONE:
                return face == 0 ? 0.2f : (face == 1 ? 0.3f : 0.2f);
            case Blocks::BlockType::COBBLESTONE_2:
                return face == 0 ? 0.3f : (face == 1 ? 0.4f : 0.3f);
            case Blocks::BlockType::SUPERBLOCK:
                return face == 0 ? 0.4f : (face == 1 ? 0.5f : 0.4f);
            default:
                return 0.0f;
        }
    }

} // namespace Barnocraft::Rendering
