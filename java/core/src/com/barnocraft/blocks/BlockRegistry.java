package com.barnocraft.blocks;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.utils.Disposable;

import java.util.HashMap;
import java.util.Map;

public class BlockRegistry implements Disposable {
    private final Map<String, BlockType> blockTypes = new HashMap<>();
    private Texture grassSideTexture;
    private Texture grassTopTexture;
    private Texture stoneTexture;
    
    public void loadTextures() {
        grassSideTexture = new Texture(Gdx.files.internal("textures/grass_side.png"));
        grassTopTexture = new Texture(Gdx.files.internal("textures/grass_top.png"));
        stoneTexture = new Texture(Gdx.files.internal("textures/stone.png"));
        
        // Create block types
        blockTypes.put("grass", BlockType.createGrass(grassSideTexture, grassTopTexture, stoneTexture));
        blockTypes.put("stone", BlockType.createStone(stoneTexture));
    }
    
    public BlockType getBlockType(String id) {
        return blockTypes.get(id);
    }
    
    @Override
    public void dispose() {
        if (grassSideTexture != null) grassSideTexture.dispose();
        if (grassTopTexture != null) grassTopTexture.dispose();
        if (stoneTexture != null) stoneTexture.dispose();
    }
}
