package com.barnocraft.world;

import com.badlogic.gdx.graphics.g3d.Model;
import com.badlogic.gdx.graphics.g3d.ModelInstance;
import com.badlogic.gdx.math.Vector3;
import com.barnocraft.Constants;

public class Chunk {
    public final int x;
    public final int z;
    private final Block[][][] blocks;
    private ModelInstance mesh;
    private boolean dirty = true;
    
    public Chunk(int x, int z) {
        this.x = x;
        this.z = z;
        this.blocks = new Block[Constants.CHUNK_SIZE][Constants.WORLD_HEIGHT][Constants.CHUNK_SIZE];
    }
    
    public void setBlock(int localX, int y, int localZ, Block block) {
        if (localX >= 0 && localX < Constants.CHUNK_SIZE && 
            y >= 0 && y < Constants.WORLD_HEIGHT && 
            localZ >= 0 && localZ < Constants.CHUNK_SIZE) {
            blocks[localX][y][localZ] = block;
            dirty = true;
        }
    }
    
    public Block getBlock(int localX, int y, int localZ) {
        if (localX >= 0 && localX < Constants.CHUNK_SIZE && 
            y >= 0 && y < Constants.WORLD_HEIGHT && 
            localZ >= 0 && localZ < Constants.CHUNK_SIZE) {
            return blocks[localX][y][localZ];
        }
        return null;
    }
    
    public void setMesh(ModelInstance mesh) {
        this.mesh = mesh;
    }
    
    public ModelInstance getMesh() {
        return mesh;
    }
    
    public boolean isDirty() {
        return dirty;
    }
    
    public void setClean() {
        dirty = false;
    }
    
    public void dispose() {
        // Mesh disposal handled by renderer
    }
}
