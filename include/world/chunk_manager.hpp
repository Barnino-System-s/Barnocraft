#pragma once

#include "chunk_coordinates.hpp"
#include "chunk.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <queue>
#include <functional>

namespace Barnocraft::Generation {
    class ChunkGenerator;
}

namespace Barnocraft::World {

    class World;

    class ChunkManager {
    public:
        using ChunkLoadedCallback = std::function<void(Chunk*)>;
        using ChunkUnloadedCallback = std::function<void(const ChunkCoordinates&)>;

    private:
        World* world;
        Generation::ChunkGenerator* chunkGenerator;
        std::unordered_map<size_t, std::unique_ptr<Chunk>> loadedChunks;
        std::unordered_map<size_t, ChunkCoordinates> generatingChunks;
        std::queue<ChunkCoordinates> generationQueue;
        Vector3i lastPlayerPosition;
        ChunkLoadedCallback onChunkLoaded;
        ChunkUnloadedCallback onChunkUnloaded;
        const int MAX_GENERATIONS_PER_FRAME = 4;

    public:
        ChunkManager(World* world, Generation::ChunkGenerator* generator);
        ~ChunkManager();

        void updatePlayerPosition(const Vector3i& playerPosition);
        void update();

        Chunk* getChunk(const ChunkCoordinates& coord);
        bool hasChunk(const ChunkCoordinates& coord) const;
        std::vector<Chunk*> getLoadedChunks() const;

        void setOnChunkLoadedCallback(ChunkLoadedCallback callback);
        void setOnChunkUnloadedCallback(ChunkUnloadedCallback callback);

        void cleanup();

    private:
        void updateChunkLoading();
        void loadOrGenerateChunk(const ChunkCoordinates& coord);
        void processGenerationQueue();
        void unloadChunk(const ChunkCoordinates& coord);
    };

} // namespace Barnocraft::World
