package com.barnocraft.world;

import com.badlogic.gdx.graphics.Mesh;
import com.badlogic.gdx.graphics.VertexAttribute;
import com.badlogic.gdx.graphics.g3d.Material;
import com.badlogic.gdx.graphics.g3d.Model;
import com.badlogic.gdx.graphics.g3d.ModelInstance;
import com.badlogic.gdx.graphics.g3d.attributes.ColorAttribute;
import com.badlogic.gdx.graphics.g3d.utils.MeshPartBuilder;
import com.badlogic.gdx.graphics.g3d.utils.ModelBuilder;
import com.badlogic.gdx.math.Vector3;
import com.barnocraft.Constants;
import com.barnocraft.blocks.BlockType;

public class ChunkMeshBuilder {
    private static final int VERTEX_SIZE = 8; // 3 position + 2 UV + 3 normal
    private static final float[] VERTICES = new float[6 * 4 * VERTEX_SIZE]; // 6 faces * 4 vertices * size
    private static final short[] INDICES = new short[6 * 6]; // 6 faces * 6 indices (2 triangles)
    
    // Face definitions: front, back, top, bottom, left, right
    private static final int[][] FACE_INDICES = {
        {0, 1, 2, 2, 3, 0}, // front
        {4, 5, 6, 6, 7, 4}, // back
        {8, 9, 10, 10, 11, 8}, // top
        {12, 13, 14, 14, 15, 12}, // bottom
        {16, 17, 18, 18, 19, 16}, // left
        {20, 21, 22, 22, 23, 20}  // right
    };
    
    // Normal vectors for each face
    private static final float[][] FACE_NORMALS = {
        {0, 0, 1},   // front
        {0, 0, -1},  // back
        {0, 1, 0},   // top
        {0, -1, 0},  // bottom
        {-1, 0, 0},  // left
        {1, 0, 0}    // right
    };
    
    // UV coordinates for each face
    private static final float[][] FACE_UVS = {
        {0, 0, 1, 0, 1, 1, 0, 1}, // front
        {1, 0, 0, 0, 0, 1, 1, 1}, // back
        {0, 1, 0, 0, 1, 0, 1, 1}, // top
        {0, 0, 1, 0, 1, 1, 0, 1}, // bottom
        {1, 0, 0, 0, 0, 1, 1, 1}, // left
        {0, 0, 1, 0, 1, 1, 0, 1}  // right
    };
    
    public static ModelInstance buildChunkMesh(Chunk chunk, BlockType grassType, BlockType stoneType) {
        ModelBuilder modelBuilder = new ModelBuilder();
        int startX = chunk.x * Constants.CHUNK_SIZE;
        int startZ = chunk.z * Constants.CHUNK_SIZE;
        
        for (int localX = 0; localX < Constants.CHUNK_SIZE; localX++) {
            for (int y = 0; y < Constants.WORLD_HEIGHT; y++) {
                for (int localZ = 0; localZ < Constants.CHUNK_SIZE; localZ++) {
                    Block block = chunk.getBlock(localX, y, localZ);
                    if (block == null || block.isAir()) continue;
                    
                    BlockType blockType = block.type;
                    if (blockType == null) continue;
                    
                    // Create cube for this block
                    float x = startX + localX;
                    float z = startZ + localZ;
                    
                    // For now, use a simple colored material
                    // In a proper implementation, we'd use the block's textures
                    Material material;
                    if (blockType.id.equals(Constants.BLOCK_GRASS)) {
                        material = new Material(ColorAttribute.createDiffuse(0.5f, 0.8f, 0.3f, 1f));
                    } else if (blockType.id.equals(Constants.BLOCK_STONE)) {
                        material = new Material(ColorAttribute.createDiffuse(0.5f, 0.5f, 0.5f, 1f));
                    } else {
                        material = new Material(ColorAttribute.createDiffuse(1f, 1f, 1f, 1f));
                    }
                    
                    modelBuilder.begin();
                    MeshPartBuilder builder = modelBuilder.part("block", primitiveType, usage, material);
                    builder.setColor(1, 1, 1, 1);
                    
                    // Build cube at position (x, y, z)
                    builder.box(x, y, z, 1, 1, 1);
                    
                    Model model = modelBuilder.end();
                    if (model != null) {
                        return new ModelInstance(model);
                    }
                }
            }
        }
        
        return null;
    }
}
