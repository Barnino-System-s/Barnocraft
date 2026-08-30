#include "blocks/block_registry.hpp"
#include "blocks/block.hpp"

namespace Barnocraft::Blocks {

    std::unordered_map<BlockType, Block> BlockRegistry::blocks;
    bool BlockRegistry::initialized = false;

    void BlockRegistry::initialize() {
        if (initialized) return;
        registerBlocks();
        initialized = true;
    }

    void BlockRegistry::registerBlocks() {
        blocks[BlockType::AIR] = Block(BlockType::AIR, "Air", false, true, false);
        blocks[BlockType::GRASS_BLOCK] = Block(BlockType::GRASS_BLOCK, "Grass Block", true, false, false);
        blocks[BlockType::DIRT] = Block(BlockType::DIRT, "Dirt", true, false, false);
        blocks[BlockType::COBBLESTONE] = Block(BlockType::COBBLESTONE, "Cobblestone", true, false, false);
        blocks[BlockType::COBBLESTONE_2] = Block(BlockType::COBBLESTONE_2, "Cobblestone 2", true, false, false);
        blocks[BlockType::SUPERBLOCK] = Block(BlockType::SUPERBLOCK, "Superblock", true, false, true);
    }

    const Block& BlockRegistry::getBlock(BlockType type) {
        initialize();
        auto it = blocks.find(type);
        if (it != blocks.end()) {
            return it->second;
        }
        return blocks.at(BlockType::AIR);
    }

    const Block& BlockRegistry::getBlockById(int id) {
        initialize();
        BlockType type = static_cast<BlockType>(id);
        return getBlock(type);
    }

} // namespace Barnocraft::Blocks
