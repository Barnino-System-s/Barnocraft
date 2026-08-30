#pragma once

#include "block.hpp"
#include "block_type.hpp"
#include <unordered_map>
#include <memory>

namespace Barnocraft::Blocks {

    class BlockRegistry {
    private:
        static std::unordered_map<BlockType, Block> blocks;
        static bool initialized;

        static void registerBlocks();

    public:
        static void initialize();
        static const Block& getBlock(BlockType type);
        static const Block& getBlockById(int id);
    };

} // namespace Barnocraft::Blocks
