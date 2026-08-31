package com.barnocraft.world;

import com.badlogic.gdx.utils.Disposable;
import com.barnocraft.Constants;
import com.barnocraft.blocks.BlockRegistry;

public class World implements Disposable {
    private final Chunk[][] chunks;
    private final BlockRegistry blockRegistry;
    
    public World(BlockRegistry blockRegistry) {
        this.blockRegistry = blockRegistry;
        this.chunks = new Chunk[Constants.CHUNKS_X][Constants.CHUNKS_Z];
        
        // Initialize chunks
        for (int cx = 0; cx < Constants.CHUNKS_X; cx++) {
            for (int cz = 0; cz < Constants.CHUNKS_Z; cz++) {
                chunks[cx][cz] = new Chunk(cx, cz);
            }
        }
        
        // Generate initial world
        generateWorld();
    }
    
    private void generateWorld() {
        for (int cx = 0; cx < Constants.CHUNKS_X; cx++) {
            for (int cz = 0; cz < Constants.CHUNKS_Z; cz++) {
                Chunk chunk = chunks[cx][cz];
                for (int localX = 0; localX < Constants.CHUNK_SIZE; localX++) {
                    for (int localZ = 0; localZ < Constants.CHUNK_SIZE; localZ++) {
                        // Generate flat terrain at y=5
                        for (int y = 0; y < Constants.WORLD_HEIGHT; y++) {
                            if (y == 5) {
                                chunk.setBlock(localX, y, localZ, 
                                    new Block(blockRegistry.getBlockType(Constants.BLOCK_GRASS)));
                            } else if (y < 5) {
                                chunk.setBlock(localX, y, localZ, 
                                    new Block(blockRegistry.getBlockType(Constants.BLOCK_STONE)));
                            } else {
                                chunk.setBlock(localX, y, localZ, null); // Air
                            }
                        }
                    }
                }
            }
        }
    }
    
    public Chunk getChunk(int cx, int cz) {
        if (cx >= 0 && cx < Constants.CHUNKS_X && cz >= 0 && cz < Constants.CHUNKS_Z) {
            return chunks[cx][cz];
        }
        return null;
    }
    
    public Block getBlock(int x, int y, int z) {
        int cx = x / Constants.CHUNK_SIZE;
        int cz = z / Constants.CHUNK_SIZE;
        int localX = x % Constants.CHUNK_SIZE;
        int localZ = z % Constants.CHUNK_SIZE;
        
        Chunk chunk = getChunk(cx, cz);
        if (chunk != null) {
            return chunk.getBlock(localX, y, localZ);
        }
        return null;
    }
    
    public void setBlock(int x, int y, int z, Block block) {
        int cx = x / Constants.CHUNK_SIZE;
        int cz = z / Constants.CHUNK_SIZE;
        int localX = x % Constants.CHUNK_SIZE;
        int localZ = z % Constants.CHUNK_SIZE;
        
        Chunk chunk = getChunk(cx, cz);
        if (chunk != null) {
            chunk.setBlock(localX, y, localZ, block);
        }
    }
    
    public boolean isSolid(int x, int y, int z) {
        Block block = getBlock(x, y, z);
        return block != null && !block.isAir();
    }
    
    public boolean isInBounds(int x, int y, int z) {
        return x >= 0 && x < Constants.WORLD_WIDTH && 
               y >= 0 && y < Constants.WORLD_HEIGHT && 
               z >= 0 && z < Constants.WORLD_DEPTH;
    }
    
    @Override
    public void dispose() {
        for (int cx = 0; cx < Constants.CHUNKS_X; cx++) {
            for (int cz = 0; cz < Constants.CHUNKS_Z; cz++) {
                if (chunks[cx][cz] != null) {
                    chunks[cx][cz].dispose();
                }
            }
        }
    }
}
