#pragma once

#include "chunk_coordinates.hpp"
#include "blocks/block_type.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace Barnocraft::World {

    class World;

    class Chunk {
    public:
        using ModifiedCallback = std::function<void(Chunk*)>;

        ChunkCoordinates coordinates;
        bool isDirty = false;
        bool needsMeshRebuild = true;

    private:
        World* world = nullptr;
        std::vector<uint8_t> blocks;
        ModifiedCallback onModified;

    public:
        Chunk(const ChunkCoordinates& coordinates, World* world, ModifiedCallback onModified = nullptr);
        Chunk(const ChunkCoordinates& coordinates, World* world, const std::vector<uint8_t>& data, ModifiedCallback onModified = nullptr);

        BlockType getBlock(int localX, int localY, int localZ) const;
        BlockType getBlock(const Vector3i& localPos) const;

        bool setBlock(int localX, int localY, int localZ, BlockType blockType);
        bool setBlock(const Vector3i& localPos, BlockType blockType);

        const std::vector<uint8_t>& getBlockData() const;

        void markAsClean();
        void markMeshAsRebuilt();

        void setOnModifiedCallback(ModifiedCallback callback);

    private:
        void markNeighborChunksForMeshRebuild(int localX, int localY, int localZ);
        int getIndex(int localX, int localY, int localZ) const;
    };

} // namespace Barnocraft::World
