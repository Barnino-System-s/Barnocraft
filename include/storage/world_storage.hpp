#pragma once

#include "world/chunk_coordinates.hpp"
#include "world/chunk.hpp"
#include <string>
#include <memory>

namespace Barnocraft::Storage {

    class WorldStorage {
    private:
        std::string worldPath;

    public:
        WorldStorage(const std::string& worldName);
        ~WorldStorage();

        bool saveChunk(const World::Chunk& chunk);
        bool loadChunk(World::Chunk& chunk, const World::ChunkCoordinates& coordinates);
        bool chunkExists(const World::ChunkCoordinates& coordinates) const;
        bool deleteChunk(const World::ChunkCoordinates& coordinates);

    private:
        std::string getChunkPath(const World::ChunkCoordinates& coordinates) const;
    };

} // namespace Barnocraft::Storage
