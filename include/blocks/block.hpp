#pragma once

#include "block_type.hpp"
#include <string>

namespace Barnocraft::Blocks {

    class Block {
    public:
        BlockType type;
        std::string name;
        bool isSolid;
        bool isTransparent;
        bool isIndestructible;

        Block(BlockType type, const std::string& name, 
              bool isSolid = true, bool isTransparent = false, bool isIndestructible = false)
            : type(type), name(name), isSolid(isSolid), 
              isTransparent(isTransparent), isIndestructible(isIndestructible) {}

        bool canBeDestroyed() const {
            return !isIndestructible;
        }

        bool canBePlacedOn(const Block& topBlock) const {
            return topBlock.isSolid || topBlock.type == BlockType::AIR;
        }
    };

} // namespace Barnocraft::Blocks
