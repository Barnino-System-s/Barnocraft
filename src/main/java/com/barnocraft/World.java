package com.barnocraft;

import com.jme3.scene.Geometry;
import com.jme3.scene.Node;
import com.jme3.scene.shape.Box;
import com.jme3.math.Vector3f;
import com.jme3.material.Material;
import com.jme3.scene.Mesh;
import com.jme3.util.BufferUtils;
import com.jme3.scene.VertexBuffer;
import com.jme3.scene.VertexBuffer.Type;
import com.jme3.scene.VertexBuffer.Format;
import com.jme3.scene.VertexBuffer.Usage;
import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Manages the game world including chunks and blocks
 */
public class World {
    
    public static final int CHUNK_SIZE = 16;
    public static final int CHUNKS_X = 5;
    public static final int CHUNKS_Z = 5;
    public static final int WORLD_WIDTH = CHUNKS_X * CHUNK_SIZE;
    public static final int WORLD_DEPTH = CHUNKS_Z * CHUNK_SIZE;
    public static final int WORLD_HEIGHT = 32;
    
    private final Node rootNode;
    private final Textures textures;
    
    // 3D array representing the world: [y][x][z]
    private final BlockType[][][] blocks;
    
    // Chunks for optimization
    private final Node[][] chunkNodes;
    
    public World(Node rootNode, Textures textures) {
        this.rootNode = rootNode;
        this.textures = textures;
        this.blocks = new BlockType[WORLD_HEIGHT][WORLD_WIDTH][WORLD_DEPTH];
        this.chunkNodes = new Node[CHUNKS_X][CHUNKS_Z];
        
        initializeWorld();
    }
    
    private void initializeWorld() {
        // Initialize all blocks to AIR
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int x = 0; x < WORLD_WIDTH; x++) {
                for (int z = 0; z < WORLD_DEPTH; z++) {
                    if (y == 5) {
                        blocks[y][x][z] = BlockType.GRASS;
                    } else if (y < 5) {
                        blocks[y][x][z] = BlockType.STONE;
                    } else {
                        blocks[y][x][z] = BlockType.AIR;
                    }
                }
            }
        }
        
        // Create chunks
        createChunks();
    }
    
    private void createChunks() {
        for (int cx = 0; cx < CHUNKS_X; cx++) {
            for (int cz = 0; cz < CHUNKS_Z; cz++) {
                chunkNodes[cx][cz] = new Node("Chunk_" + cx + "_" + cz);
                rootNode.attachChild(chunkNodes[cx][cz]);
                buildChunk(cx, cz);
            }
        }
    }
    
    private void buildChunk(int cx, int cz) {
        Node chunkNode = chunkNodes[cx][cz];
        
        // Clear existing chunk
        chunkNode.detachAllChildren();
        
        int startX = cx * CHUNK_SIZE;
        int startZ = cz * CHUNK_SIZE;
        
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int x = startX; x < startX + CHUNK_SIZE; x++) {
                for (int z = startZ; z < startZ + CHUNK_SIZE; z++) {
                    BlockType block = blocks[y][x][z];
                    if (block != BlockType.AIR) {
                        createBlockGeometry(x, y, z, block, chunkNode);
                    }
                }
            }
        }
    }
    
    private void createBlockGeometry(int x, int y, int z, BlockType blockType, Node parentNode) {
        // Create a box mesh for the block
        Box blockShape = new Box(0.5f, 0.5f, 0.5f);
        Geometry blockGeo = new Geometry("Block_" + x + "_" + y + "_" + z, blockShape);
        
        blockGeo.setMaterial(textures.getMaterial(blockType));
        blockGeo.setLocalTranslation(x, y, z);
        
        parentNode.attachChild(blockGeo);
    }
    
    public BlockType getBlock(int x, int y, int z) {
        if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT || z < 0 || z >= WORLD_DEPTH) {
            return BlockType.AIR;
        }
        return blocks[y][x][z];
    }
    
    public void setBlock(int x, int y, int z, BlockType blockType) {
        if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT || z < 0 || z >= WORLD_DEPTH) {
            return;
        }
        
        blocks[y][x][z] = blockType;
        
        // Update the corresponding chunk
        int cx = Math.min(Math.max(x / CHUNK_SIZE, 0), CHUNKS_X - 1);
        int cz = Math.min(Math.max(z / CHUNK_SIZE, 0), CHUNKS_Z - 1);
        buildChunk(cx, cz);
    }
    
    public boolean isSolid(int x, int y, int z) {
        if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT || z < 0 || z >= WORLD_DEPTH) {
            return false;
        }
        return getBlock(x, y, z).isSolid();
    }
    
    public void cleanup() {
        for (int cx = 0; cx < CHUNKS_X; cx++) {
            for (int cz = 0; cz < CHUNKS_Z; cz++) {
                if (chunkNodes[cx][cz] != null) {
                    chunkNodes[cx][cz].detachAllChildren();
                }
            }
        }
    }
}
