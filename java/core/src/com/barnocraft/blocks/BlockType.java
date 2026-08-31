package com.barnocraft.blocks;

import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class BlockType {
    public final String id;
    public final TextureRegion[] textures; // [front, back, top, bottom, left, right]
    public final boolean isOpaque;
    
    public BlockType(String id, TextureRegion[] textures, boolean isOpaque) {
        this.id = id;
        this.textures = textures;
        this.isOpaque = isOpaque;
    }
    
    public static BlockType createGrass(Texture grassSide, Texture grassTop, Texture stone) {
        TextureRegion sideRegion = new TextureRegion(grassSide);
        TextureRegion topRegion = new TextureRegion(grassTop);
        TextureRegion bottomRegion = new TextureRegion(stone);
        
        TextureRegion[] textures = new TextureRegion[] {
            sideRegion,  // front
            sideRegion,  // back
            topRegion,   // top
            bottomRegion, // bottom
            sideRegion,  // left
            sideRegion   // right
        };
        
        return new BlockType("grass", textures, true);
    }
    
    public static BlockType createStone(Texture stone) {
        TextureRegion region = new TextureRegion(stone);
        TextureRegion[] textures = new TextureRegion[] {
            region, region, region, region, region, region
        };
        return new BlockType("stone", textures, true);
    }
}
