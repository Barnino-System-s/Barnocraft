#pragma once

#include "world/chunk_coordinates.hpp"
#include "world/chunk.hpp"
#include <string>
#include <memory>

namespace Barnocraft::World {
    class World;
}

namespace Barnocraft::Generation {

    class ChunkGenerator {
    private:
        std::string seed;
        uint32_t deterministicSeed;

    public:
        ChunkGenerator(const std::string& seed);
        ~ChunkGenerator();

        void generateChunk(World::Chunk& chunk);

    private:
        int generateHeight(int worldX, int worldZ) const;
        uint32_t getDeterministicHash(int x, int z) const;
        void generateColumn(World::Chunk& chunk, int localX, int localZ, int surfaceHeight);
    };

} // namespace Barnocraft::Generation
