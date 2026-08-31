package com.barnocraft.rendering;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Camera;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.Mesh;
import com.badlogic.gdx.graphics.VertexAttribute;
import com.badlogic.gdx.graphics.g3d.Environment;
import com.badlogic.gdx.graphics.g3d.Material;
import com.badlogic.gdx.graphics.g3d.Model;
import com.badlogic.gdx.graphics.g3d.ModelBatch;
import com.badlogic.gdx.graphics.g3d.ModelInstance;
import com.badlogic.gdx.graphics.g3d.attributes.ColorAttribute;
import com.badlogic.gdx.graphics.g3d.environment.DirectionalLight;
import com.badlogic.gdx.graphics.g3d.utils.ModelBuilder;
import com.badlogic.gdx.math.Vector3;
import com.barnocraft.Constants;
import com.barnocraft.blocks.BlockType;
import com.barnocraft.world.Chunk;
import com.barnocraft.world.World;

public class WorldRenderer {
    private final World world;
    private final ModelBatch modelBatch;
    private final Environment environment;
    private ModelInstance highlightMesh;
    private boolean showHighlight = false;
    
    public WorldRenderer(World world) {
        this.world = world;
        this.modelBatch = new ModelBatch();
        
        // Setup environment
        this.environment = new Environment();
        environment.set(new ColorAttribute(ColorAttribute.AmbientLight, 0.7f, 0.7f, 0.7f, 1f));
        
        DirectionalLight sun = new DirectionalLight();
        sun.setColor(1f, 1f, 1f, 1f);
        sun.direction.set(60f, 100f, 40f).nor();
        environment.add(sun);
        
        // Create highlight mesh
        createHighlightMesh();
    }
    
    private void createHighlightMesh() {
        ModelBuilder modelBuilder = new ModelBuilder();
        Model boxModel = modelBuilder.createBox(
            1.003f, 1.003f, 1.003f,
            new Material(ColorAttribute.createDiffuse(0.07f, 0.07f, 0.07f, 0.8f)),
            VertexAttribute.Position | VertexAttribute.ColorUnpacked
        );
        highlightMesh = new ModelInstance(boxModel);
        highlightMesh.transform.setToTranslation(0, 0, 0);
    }
    
    public void render(Camera camera) {
        Gdx.gl.glClearColor(
            Constants.SKY_COLOR.r,
            Constants.SKY_COLOR.g,
            Constants.SKY_COLOR.b,
            1f
        );
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT | GL20.GL_DEPTH_BUFFER_BIT);
        
        modelBatch.begin(camera);
        
        // Render chunks
        for (int cx = 0; cx < Constants.CHUNKS_X; cx++) {
            for (int cz = 0; cz < Constants.CHUNKS_Z; cz++) {
                Chunk chunk = world.getChunk(cx, cz);
                if (chunk != null && chunk.getMesh() != null) {
                    modelBatch.render(chunk.getMesh(), environment);
                }
            }
        }
        
        // Render highlight
        if (showHighlight && highlightMesh != null) {
            modelBatch.render(highlightMesh, environment);
        }
        
        modelBatch.end();
    }
    
    public void updateHighlight(int x, int y, int z) {
        if (highlightMesh != null) {
            highlightMesh.transform.setToTranslation(x + 0.5f, y + 0.5f, z + 0.5f);
            showHighlight = true;
        }
    }
    
    public void hideHighlight() {
        showHighlight = false;
    }
    
    public void rebuildChunkMesh(Chunk chunk, BlockType grassType, BlockType stoneType) {
        ModelBuilder modelBuilder = new ModelBuilder();
        
        int startX = chunk.x * Constants.CHUNK_SIZE;
        int startZ = chunk.z * Constants.CHUNK_SIZE;
        
        for (int localX = 0; localX < Constants.CHUNK_SIZE; localX++) {
            for (int y = 0; y < Constants.WORLD_HEIGHT; y++) {
                for (int localZ = 0; localZ < Constants.CHUNK_SIZE; localZ++) {
                    com.barnocraft.world.Block block = chunk.getBlock(localX, y, localZ);
                    if (block == null || block.isAir()) continue;
                    
                    BlockType blockType = block.type;
                    if (blockType == null) continue;
                    
                    // Create cube for this block
                    float x = startX + localX;
                    float z = startZ + localZ;
                    
                    Material material = new Material();
                    // Use the first texture for now (will be improved with proper UV mapping)
                    
                    modelBuilder.createBox(
                        1f, 1f, 1f,
                        material,
                        VertexAttribute.Position | VertexAttribute.Normal
                    );
                }
            }
        }
        
        Model model = modelBuilder.end();
        if (model != null) {
            chunk.setMesh(new ModelInstance(model));
            chunk.setClean();
        }
    }
    
    public void dispose() {
        modelBatch.dispose();
        if (highlightMesh != null) {
            highlightMesh.model.dispose();
        }
    }
}
