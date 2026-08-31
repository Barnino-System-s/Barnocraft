package com.barnocraft.rendering;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Color;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.scenes.scene2d.Stage;
import com.badlogic.gdx.scenes.scene2d.ui.Image;
import com.badlogic.gdx.scenes.scene2d.ui.Label;
import com.badlogic.gdx.scenes.scene2d.ui.Table;
import com.badlogic.gdx.utils.viewport.ScreenViewport;

public class HUD {
    private final SpriteBatch batch;
    private final BitmapFont font;
    private final Stage stage;
    private final Table hotbarTable;
    private Texture crosshairTexture;
    private boolean showInfo = true;
    private boolean showHotbar = true;
    private Image[] hotbarSlots = new Image[2];
    private Image selectedIndicator;
    private int selectedSlot = 1;
    
    public HUD() {
        this.batch = new SpriteBatch();
        this.font = new BitmapFont();
        this.stage = new Stage(new ScreenViewport());
        
        // Create hotbar
        this.hotbarTable = new Table();
        hotbarTable.setPosition(Gdx.graphics.getWidth() / 2f - 100, 16);
        hotbarTable.pad(6).background("hotbar_bg");
        
        // Create crosshair
        createCrosshair();
        
        // Setup hotbar slots
        setupHotbar();
    }
    
    private void createCrosshair() {
        // Create a simple white crosshair texture programmatically
        // In a real implementation, you'd load this from assets
    }
    
    private void setupHotbar() {
        // Load textures for hotbar slots
        // This would be replaced with actual texture loading
        for (int i = 0; i < 2; i++) {
            hotbarSlots[i] = new Image();
            hotbarSlots[i].setSize(36, 36);
        }
        
        selectedIndicator = new Image();
        selectedIndicator.setSize(36, 36);
        selectedIndicator.setColor(Color.WHITE);
        
        hotbarTable.add(hotbarSlots[0]).size(36, 36).pad(2);
        hotbarTable.add(hotbarSlots[1]).size(36, 36).pad(2);
        stage.addActor(hotbarTable);
    }
    
    public void render() {
        // Render HUD
        batch.begin();
        
        // Draw crosshair if mouse is locked
        if (!showInfo) {
            drawCrosshair(batch);
        }
        
        // Draw info text
        if (showInfo) {
            font.setColor(Color.WHITE);
            font.draw(batch, 
                "Click to capture mouse | WASD - move | Space - jump | 1/2 - select block | LMB - break | RMB - place",
                Gdx.graphics.getWidth() / 2f - 300,
                70
            );
        }
        
        batch.end();
        
        // Render stage (hotbar)
        stage.draw();
    }
    
    private void drawCrosshair(SpriteBatch batch) {
        // Draw simple crosshair
        float centerX = Gdx.graphics.getWidth() / 2f;
        float centerY = Gdx.graphics.getHeight() / 2f;
        
        // Horizontal line
        batch.setColor(Color.WHITE);
        batch.draw(
            new TextureRegion(new Texture("white.png")),
            centerX - 6, centerY - 1, 12, 2
        );
        
        // Vertical line
        batch.draw(
            new TextureRegion(new Texture("white.png")),
            centerX - 1, centerY - 6, 2, 12
        );
    }
    
    public void setSelectedSlot(int slot) {
        this.selectedSlot = slot;
        // Update visual selection
    }
    
    public void showInfo(boolean show) {
        this.showInfo = show;
    }
    
    public void showHotbar(boolean show) {
        this.showHotbar = show;
        hotbarTable.setVisible(show);
    }
    
    public void resize(int width, int height) {
        stage.getViewport().update(width, height, true);
        hotbarTable.setPosition(width / 2f - 100, 16);
    }
    
    public void dispose() {
        batch.dispose();
        font.dispose();
        stage.dispose();
    }
}
