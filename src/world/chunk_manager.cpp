#include "world/chunk_manager.hpp"
#include "world/world.hpp"
#include "generation/chunk_generator.hpp"
#include "world/world_constants.hpp"
#include <algorithm>

namespace Barnocraft::World {

    ChunkManager::ChunkManager(World* w, Generation::ChunkGenerator* generator)
        : world(w), chunkGenerator(generator), lastPlayerPosition(0, 0, 0) {}

    ChunkManager::~ChunkManager() {
        cleanup();
    }

    void ChunkManager::updatePlayerPosition(const Vector3i& playerPosition) {
        lastPlayerPosition = playerPosition;
    }

    void ChunkManager::update() {
        updateChunkLoading();
        processGenerationQueue();
    }

    void ChunkManager::updateChunkLoading() {
        ChunkCoordinates playerChunk = ChunkCoordinates::from_world_position_int(
            lastPlayerPosition.x, lastPlayerPosition.z);

        // Determine required chunks
        std::unordered_map<size_t, ChunkCoordinates> requiredChunks;

        for (int x = -CHUNK_LOAD_DISTANCE; x <= CHUNK_LOAD_DISTANCE; x++) {
            for (int z = -CHUNK_LOAD_DISTANCE; z <= CHUNK_LOAD_DISTANCE; z++) {
                ChunkCoordinates coord(playerChunk.x + x, playerChunk.z + z);
                requiredChunks[coord.hash()] = coord;
            }
        }

        // Unload distant chunks
        std::vector<ChunkCoordinates> chunksToUnload;
        for (const auto& pair : loadedChunks) {
            if (requiredChunks.find(pair.first) == requiredChunks.end()) {
                chunksToUnload.push_back(pair.second->coordinates);
            }
        }

        for (const auto& coord : chunksToUnload) {
            unloadChunk(coord);
        }

        // Load required chunks
        for (const auto& pair : requiredChunks) {
            const ChunkCoordinates& coord = pair.second;
            if (loadedChunks.find(coord.hash()) == loadedChunks.end() &&
                generatingChunks.find(coord.hash()) == generatingChunks.end()) {
                loadOrGenerateChunk(coord);
            }
        }
    }

    void ChunkManager::loadOrGenerateChunk(const ChunkCoordinates& coord) {
        generationQueue.push(coord);
        generatingChunks[coord.hash()] = coord;
    }

    void ChunkManager::processGenerationQueue() {
        int count = std::min(MAX_GENERATIONS_PER_FRAME, static_cast<int>(generationQueue.size()));

        for (int i = 0; i < count; i++) {
            if (generationQueue.empty()) break;

            ChunkCoordinates coord = generationQueue.front();
            generationQueue.pop();
            generatingChunks.erase(coord.hash());

            try {
                std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(
                    coord, world, [this](Chunk* c) {
                        // Mark for mesh rebuild
                    });
                chunkGenerator->generateChunk(*chunk);
                loadedChunks[coord.hash()] = std::move(chunk);
                if (onChunkLoaded) {
                    onChunkLoaded(loadedChunks[coord.hash()].get());
                }
            } catch (const std::exception& e) {
                // Log error
            }
        }
    }

    Chunk* ChunkManager::getChunk(const ChunkCoordinates& coord) {
        auto it = loadedChunks.find(coord.hash());
        if (it != loadedChunks.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    bool ChunkManager::hasChunk(const ChunkCoordinates& coord) const {
        return loadedChunks.find(coord.hash()) != loadedChunks.end();
    }

    std::vector<Chunk*> ChunkManager::getLoadedChunks() const {
        std::vector<Chunk*> result;
        for (const auto& pair : loadedChunks) {
            result.push_back(pair.second.get());
        }
        return result;
    }

    void ChunkManager::setOnChunkLoadedCallback(ChunkLoadedCallback callback) {
        onChunkLoaded = callback;
    }

    void ChunkManager::setOnChunkUnloadedCallback(ChunkUnloadedCallback callback) {
        onChunkUnloaded = callback;
    }

    void ChunkManager::cleanup() {
        for (auto& pair : loadedChunks) {
            if (pair.second->isDirty) {
                // TODO: Save through WorldStorage
            }
        }
        loadedChunks.clear();
        std::queue<ChunkCoordinates> empty;
        std::swap(generationQueue, empty);
        generatingChunks.clear();
    }

    void ChunkManager::unloadChunk(const ChunkCoordinates& coord) {
        auto it = loadedChunks.find(coord.hash());
        if (it != loadedChunks.end()) {
            if (it->second->isDirty) {
                // TODO: Save through WorldStorage
            }
            if (onChunkUnloaded) {
                onChunkUnloaded(coord);
            }
            loadedChunks.erase(it);
        }
    }

} // namespace Barnocraft::World
