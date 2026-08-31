package com.barnocraft;

public class Constants {
    // World constants
    public static final int CHUNK_SIZE = 16;
    public static final int CHUNKS_X = 5;
    public static final int CHUNKS_Z = 5;
    public static final int WORLD_WIDTH = CHUNKS_X * CHUNK_SIZE;
    public static final int WORLD_DEPTH = CHUNKS_Z * CHUNK_SIZE;
    public static final int WORLD_HEIGHT = 32;
    
    // Player constants
    public static final float PLAYER_WIDTH = 0.6f;
    public static final float PLAYER_HEIGHT = 1.8f;
    public static final float PLAYER_EYE_HEIGHT = 1.62f;
    public static final float PLAYER_SPEED = 4.0f;
    public static final float JUMP_SPEED = 8.0f;
    public static final float GRAVITY = -25.0f;
    
    // Rendering constants
    public static final float VIEW_DISTANCE = 150f;
    public static final float FOV = 75f;
    
    // Block types
    public static final String BLOCK_GRASS = "grass";
    public static final String BLOCK_STONE = "stone";
    
    // Colors
    public static final com.badlogic.gdx.graphics.Color SKY_COLOR = new com.badlogic.gdx.graphics.Color(0x87ceebff);
    public static final com.badlogic.gdx.graphics.Color FOG_COLOR = new com.badlogic.gdx.graphics.Color(0x87ceebff);
}
