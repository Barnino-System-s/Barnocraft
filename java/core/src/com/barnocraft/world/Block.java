package com.barnocraft.world;

import com.barnocraft.blocks.BlockType;

public class Block {
    public final BlockType type;
    
    public Block(BlockType type) {
        this.type = type;
    }
    
    public boolean isAir() {
        return type == null;
    }
}
