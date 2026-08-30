#include "generation/chunk_generator.hpp"
#include "world/world.hpp"
#include "world/world_constants.hpp"
#include "blocks/block_type.hpp"
#include <functional>
#include <cstdint>

namespace Barnocraft::Generation {

    // Simple hash function for deterministic generation
    inline uint32_t hashCombine(uint32_t seed, uint32_t value) {
        return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    }

    ChunkGenerator::ChunkGenerator(const std::string& p_seed) : seed(p_seed) {
        // Create deterministic seed from string
        deterministicSeed = 0;
        for (char c : seed) {
            deterministicSeed = hashCombine(deterministicSeed, static_cast<uint32_t>(c));
        }
    }

    ChunkGenerator::~ChunkGenerator() {}

    void ChunkGenerator::generateChunk(World::Chunk& chunk) {
        for (int localX = 0; localX < World::CHUNK_SIZE_X; localX++) {
            for (int localZ = 0; localZ < World::CHUNK_SIZE_Z; localZ++) {
                int worldX = chunk.coordinates.x * World::CHUNK_SIZE_X + localX;
                int worldZ = chunk.coordinates.z * World::CHUNK_SIZE_Z + localZ;

                int surfaceHeight = generateHeight(worldX, worldZ);
                generateColumn(chunk, localX, localZ, surfaceHeight);
            }
        }
    }

    int ChunkGenerator::generateHeight(int worldX, int worldZ) const {
        uint32_t hash = getDeterministicHash(worldX, worldZ);
        
        // Simple pseudo-random from hash
        uint32_t pseudoRand = hash * 1103515245 + 12345;
        pseudoRand = (pseudoRand / 65536) % 32768;
        
        // Convert to range [-HEIGHT_VARIATION, HEIGHT_VARIATION]
        int variation = static_cast<int>(pseudoRand) % (2 * World::HEIGHT_VARIATION + 1) - World::HEIGHT_VARIATION;
        
        int height = World::BASE_HEIGHT + variation;
        return std::clamp(height, World::MIN_GENERATION_HEIGHT, World::MAX_GENERATION_HEIGHT);
    }

    uint32_t ChunkGenerator::getDeterministicHash(int x, int z) const {
        uint32_t hash = deterministicSeed;
        hash = hashCombine(hash, static_cast<uint32_t>(x));
        hash = hashCombine(hash, static_cast<uint32_t>(z));
        hash = hashCombine(hash, 0xDEADBEEF);
        return hash;
    }

    void ChunkGenerator::generateColumn(World::Chunk& chunk, int localX, int localZ, int surfaceHeight) {
        for (int localY = 0; localY < World::CHUNK_SIZE_Y; localY++) {
            int worldY = localY;

            if (worldY > surfaceHeight) {
                chunk.setBlock(localX, localY, localZ, Blocks::BlockType::AIR);
            } else if (worldY == surfaceHeight) {
                chunk.setBlock(localX, localY, localZ, Blocks::BlockType::GRASS_BLOCK);
            } else if (worldY > surfaceHeight - World::GRASS_LAYER_HEIGHT - World::DIRT_LAYERS) {
                chunk.setBlock(localX, localY, localZ, Blocks::BlockType::DIRT);
            } else if (worldY > surfaceHeight - World::GRASS_LAYER_HEIGHT - World::DIRT_LAYERS - World::COBBLESTONE_LAYERS) {
                chunk.setBlock(localX, localY, localZ, Blocks::BlockType::COBBLESTONE);
            } else if (worldY > surfaceHeight - World::GRASS_LAYER_HEIGHT - World::DIRT_LAYERS - 
                       World::COBBLESTONE_LAYERS - World::COBBLESTONE_2_LAYERS) {
                chunk.setBlock(localX, localY, localZ, Blocks::BlockType::COBBLESTONE_2);
            } else {
                chunk.setBlock(localX, localY, localZ, Blocks::BlockType::SUPERBLOCK);
            }
        }
    }

} // namespace Barnocraft::Generation
