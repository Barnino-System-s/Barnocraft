package com.barnocraft;

/**
 * Types of blocks available in the game
 */
public enum BlockType {
    GRASS("grass"),
    STONE("stone"),
    AIR(null);
    
    private final String name;
    
    BlockType(String name) {
        this.name = name;
    }
    
    public String getName() {
        return name;
    }
    
    public boolean isSolid() {
        return this != AIR;
    }
}
