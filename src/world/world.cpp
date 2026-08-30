#include "world/world.hpp"
#include "world/chunk_manager.hpp"
#include "generation/chunk_generator.hpp"
#include "storage/world_storage.hpp"
#include "blocks/block_registry.hpp"
#include "world/world_constants.hpp"

namespace Barnocraft::World {

    World::World(const std::string& p_seed)
        : seed(p_seed) {
        Blocks::BlockRegistry::initialize();
        chunkGenerator = std::make_unique<Generation::ChunkGenerator>(seed);
        chunkManager = std::make_unique<ChunkManager>(this, chunkGenerator.get());
        storage = std::make_unique<Storage::WorldStorage>("world_" + seed);
    }

    World::~World() {
        // Cleanup order matters
        if (chunkManager) {
            chunkManager->cleanup();
        }
    }

    ChunkManager* World::getChunkManager() const {
        return chunkManager.get();
    }

    Generation::ChunkGenerator* World::getChunkGenerator() const {
        return chunkGenerator.get();
    }

    Storage::WorldStorage* World::getStorage() const {
        return storage.get();
    }

    Blocks::BlockType World::getBlock(int worldX, int worldY, int worldZ) const {
        ChunkCoordinates chunkCoord = ChunkCoordinates::from_world_position_int(worldX, worldZ);
        Vector3i localPos = ChunkCoordinates::get_local_position(Vector3i(worldX, worldY, worldZ));

        Chunk* chunk = chunkManager->getChunk(chunkCoord);
        if (chunk == nullptr) {
            return Blocks::BlockType::AIR;
        }

        return chunk->getBlock(localPos);
    }

    Blocks::BlockType World::getBlock(const Vector3i& worldPos) const {
        return getBlock(worldPos.x, worldPos.y, worldPos.z);
    }

    bool World::setBlock(int worldX, int worldY, int worldZ, Blocks::BlockType blockType) {
        ChunkCoordinates chunkCoord = ChunkCoordinates::from_world_position_int(worldX, worldZ);
        Vector3i localPos = ChunkCoordinates::get_local_position(Vector3i(worldX, worldY, worldZ));

        Chunk* chunk = chunkManager->getChunk(chunkCoord);
        if (chunk == nullptr) {
            return false;
        }

        Blocks::BlockType oldType = chunk->getBlock(localPos);
        const Blocks::Block& oldBlock = Blocks::BlockRegistry::getBlock(oldType);

        // Check SUPERBLOCK
        if (oldBlock.isIndestructible && blockType != oldType) {
            return false;
        }

        const Blocks::Block& newBlock = Blocks::BlockRegistry::getBlock(blockType);
        if (newBlock.isIndestructible && oldType != Blocks::BlockType::AIR) {
            return false;
        }

        bool success = chunk->setBlock(localPos, blockType);
        if (success && onBlockChanged) {
            onBlockChanged(Vector3i(worldX, worldY, worldZ), oldType, blockType);
        }
        return success;
    }

    bool World::setBlock(const Vector3i& worldPos, Blocks::BlockType blockType) {
        return setBlock(worldPos.x, worldPos.y, worldPos.z, blockType);
    }

    bool World::destroyBlock(int worldX, int worldY, int worldZ) {
        return setBlock(worldX, worldY, worldZ, Blocks::BlockType::AIR);
    }

    bool World::destroyBlock(const Vector3i& worldPos) {
        return destroyBlock(worldPos.x, worldPos.y, worldPos.z);
    }

    void World::markChunkForMeshRebuild(const ChunkCoordinates& chunkCoord) {
        Chunk* chunk = chunkManager->getChunk(chunkCoord);
        if (chunk != nullptr) {
            chunk->needsMeshRebuild = true;
        }
    }

    void World::update(const Vector3i& playerPosition) {
        chunkManager->updatePlayerPosition(playerPosition);
        chunkManager->update();
    }

    void World::setOnBlockChangedCallback(BlockChangedCallback callback) {
        onBlockChanged = callback;
    }

    const std::string& World::getSeed() const {
        return seed;
    }

    Vector3i World::getLocalPosition(int worldX, int worldY, int worldZ) const {
        int localX = worldX % CHUNK_SIZE_X;
        if (localX < 0) localX += CHUNK_SIZE_X;

        int localY = worldY % CHUNK_SIZE_Y;
        if (localY < 0) localY += CHUNK_SIZE_Y;

        int localZ = worldZ % CHUNK_SIZE_Z;
        if (localZ < 0) localZ += CHUNK_SIZE_Z;

        return Vector3i(localX, localY, localZ);
    }

} // namespace Barnocraft::World
