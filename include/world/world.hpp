#pragma once

#include "chunk_coordinates.hpp"
#include "chunk.hpp"
#include "blocks/block_type.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace Barnocraft::Generation {
    class ChunkGenerator;
}

namespace Barnocraft::Storage {
    class WorldStorage;
}

namespace Barnocraft::World {

    class ChunkManager;

    class World {
    public:
        using BlockChangedCallback = std::function<void(const Vector3i&, Blocks::BlockType, Blocks::BlockType)>;

    private:
        std::string seed;
        std::unique_ptr<ChunkManager> chunkManager;
        std::unique_ptr<Generation::ChunkGenerator> chunkGenerator;
        std::unique_ptr<Storage::WorldStorage> storage;
        BlockChangedCallback onBlockChanged;

    public:
        World(const std::string& seed);
        ~World();

        ChunkManager* getChunkManager() const;
        Generation::ChunkGenerator* getChunkGenerator() const;
        Storage::WorldStorage* getStorage() const;

        Blocks::BlockType getBlock(int worldX, int worldY, int worldZ) const;
        Blocks::BlockType getBlock(const Vector3i& worldPos) const;

        bool setBlock(int worldX, int worldY, int worldZ, Blocks::BlockType blockType);
        bool setBlock(const Vector3i& worldPos, Blocks::BlockType blockType);

        bool destroyBlock(int worldX, int worldY, int worldZ);
        bool destroyBlock(const Vector3i& worldPos);

        void markChunkForMeshRebuild(const ChunkCoordinates& chunkCoord);

        void update(const Vector3i& playerPosition);

        void setOnBlockChangedCallback(BlockChangedCallback callback);

        const std::string& getSeed() const;

    private:
        Vector3i getLocalPosition(int worldX, int worldY, int worldZ) const;
    };

} // namespace Barnocraft::World
