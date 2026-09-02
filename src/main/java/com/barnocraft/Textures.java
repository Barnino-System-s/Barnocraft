package com.barnocraft;

import com.jme3.asset.AssetManager;
import com.jme3.material.Material;
import com.jme3.material.RenderState;
import com.jme3.math.ColorRGBA;
import com.jme3.texture.Texture;
import com.jme3.texture.Texture2D;

import java.util.HashMap;
import java.util.Map;

/**
 * Manages texture loading and material creation for blocks
 */
public class Textures {
    
    private final AssetManager assetManager;
    private final Map<BlockType, Material> blockMaterials;
    
    public Textures(AssetManager assetManager) {
        this.assetManager = assetManager;
        this.blockMaterials = new HashMap<>();
        
        // Load textures and create materials
        loadTextures();
    }
    
    private void loadTextures() {
        // Create materials for each block type
        
        // Grass block material (using a simple color for now)
        Material grassMaterial = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
        grassMaterial.setColor("Color", ColorRGBA.Green);
        blockMaterials.put(BlockType.GRASS, grassMaterial);
        
        // Stone block material
        Material stoneMaterial = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
        stoneMaterial.setColor("Color", ColorRGBA.Gray);
        blockMaterials.put(BlockType.STONE, stoneMaterial);
        
        // Try to load actual textures if available
        try {
            Texture grassTexture = assetManager.loadTexture("assets/textures/grass_2.png");
            Material texturedGrass = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
            texturedGrass.setTexture("ColorMap", grassTexture);
            blockMaterials.put(BlockType.GRASS, texturedGrass);
        } catch (Exception e) {
            System.out.println("Could not load grass texture, using fallback color");
        }
        
        try {
            Texture stoneTexture = assetManager.loadTexture("assets/textures/stone.png");
            Material texturedStone = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
            texturedStone.setTexture("ColorMap", stoneTexture);
            blockMaterials.put(BlockType.STONE, texturedStone);
        } catch (Exception e) {
            System.out.println("Could not load stone texture, using fallback color");
        }
    }
    
    public Material getMaterial(BlockType blockType) {
        return blockMaterials.getOrDefault(blockType, createDefaultMaterial());
    }
    
    private Material createDefaultMaterial() {
        Material defaultMaterial = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
        defaultMaterial.setColor("Color", ColorRGBA.Pink);
        return defaultMaterial;
    }
    
    public void cleanup() {
        blockMaterials.clear();
    }
}
