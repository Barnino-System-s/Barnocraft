#include "world/chunk.hpp"
#include "world/world.hpp"
#include "blocks/block_registry.hpp"
#include "world/world_constants.hpp"
#include <stdexcept>

namespace Barnocraft::World {

    Chunk::Chunk(const ChunkCoordinates& coords, World* w, ModifiedCallback callback)
        : coordinates(coords), world(w), onModified(callback) {
        blocks.resize(CHUNK_VOLUME, static_cast<uint8_t>(Blocks::BlockType::AIR));
    }

    Chunk::Chunk(const ChunkCoordinates& coords, World* w, const std::vector<uint8_t>& data, ModifiedCallback callback)
        : coordinates(coords), world(w), blocks(data), onModified(callback) {
        if (data.size() != CHUNK_VOLUME) {
            throw std::invalid_argument("Block data size mismatch");
        }
    }

    BlockType Chunk::getBlock(int localX, int localY, int localZ) const {
        if (localX < 0 || localX >= CHUNK_SIZE_X ||
            localY < 0 || localY >= CHUNK_SIZE_Y ||
            localZ < 0 || localZ >= CHUNK_SIZE_Z) {
            return Blocks::BlockType::AIR;
        }
        return static_cast<Blocks::BlockType>(blocks[getIndex(localX, localY, localZ)]);
    }

    BlockType Chunk::getBlock(const Vector3i& localPos) const {
        return getBlock(localPos.x, localPos.y, localPos.z);
    }

    bool Chunk::setBlock(int localX, int localY, int localZ, Blocks::BlockType blockType) {
        if (localX < 0 || localX >= CHUNK_SIZE_X ||
            localY < 0 || localY >= CHUNK_SIZE_Y ||
            localZ < 0 || localZ >= CHUNK_SIZE_Z) {
            return false;
        }

        int index = getIndex(localX, localY, localZ);
        Blocks::BlockType oldType = static_cast<Blocks::BlockType>(blocks[index]);
        const Blocks::Block& oldBlock = Blocks::BlockRegistry::getBlock(oldType);
        const Blocks::Block& newBlock = Blocks::BlockRegistry::getBlock(blockType);

        // Check if we can modify this block
        if (oldBlock.isIndestructible && blockType != oldType) {
            return false;
        }

        if (newBlock.isIndestructible && oldType != Blocks::BlockType::AIR) {
            return false;
        }

        blocks[index] = static_cast<uint8_t>(blockType);
        isDirty = true;
        needsMeshRebuild = true;

        // Mark neighbor chunks for mesh rebuild if on edge
        if (localX == 0 || localX == CHUNK_SIZE_X - 1 ||
            localY == 0 || localY == CHUNK_SIZE_Y - 1 ||
            localZ == 0 || localZ == CHUNK_SIZE_Z - 1) {
            markNeighborChunksForMeshRebuild(localX, localY, localZ);
        }

        if (onModified) {
            onModified(this);
        }

        return true;
    }

    bool Chunk::setBlock(const Vector3i& localPos, Blocks::BlockType blockType) {
        return setBlock(localPos.x, localPos.y, localPos.z, blockType);
    }

    const std::vector<uint8_t>& Chunk::getBlockData() const {
        return blocks;
    }

    void Chunk::markAsClean() {
        isDirty = false;
    }

    void Chunk::markMeshAsRebuilt() {
        needsMeshRebuild = false;
    }

    void Chunk::setOnModifiedCallback(ModifiedCallback callback) {
        onModified = callback;
    }

    void Chunk::markNeighborChunksForMeshRebuild(int localX, int localY, int localZ) {
        if (world == nullptr) return;

        if (localX == 0) {
            ChunkCoordinates neighborCoord(coordinates.x - 1, coordinates.z);
            world->markChunkForMeshRebuild(neighborCoord);
        } else if (localX == CHUNK_SIZE_X - 1) {
            ChunkCoordinates neighborCoord(coordinates.x + 1, coordinates.z);
            world->markChunkForMeshRebuild(neighborCoord);
        }

        if (localZ == 0) {
            ChunkCoordinates neighborCoord(coordinates.x, coordinates.z - 1);
            world->markChunkForMeshRebuild(neighborCoord);
        } else if (localZ == CHUNK_SIZE_Z - 1) {
            ChunkCoordinates neighborCoord(coordinates.x, coordinates.z + 1);
            world->markChunkForMeshRebuild(neighborCoord);
        }
    }

    int Chunk::getIndex(int localX, int localY, int localZ) const {
        return localX + localZ * CHUNK_SIZE_X + localY * CHUNK_SIZE_X * CHUNK_SIZE_Z;
    }

} // namespace Barnocraft::World
