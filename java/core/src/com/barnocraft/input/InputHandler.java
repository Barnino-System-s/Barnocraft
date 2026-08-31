package com.barnocraft.input;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.InputProcessor;
import com.barnocraft.Constants;
import com.barnocraft.player.CameraController;
import com.barnocraft.player.Player;
import com.barnocraft.world.World;

public class InputHandler implements InputProcessor {
    private final Player player;
    private final CameraController cameraController;
    private final World world;
    private final Runnable onBreakBlock;
    private final Runnable onPlaceBlock;
    private String selectedBlockType = Constants.BLOCK_STONE;
    private boolean[] keys = new boolean[256];
    private boolean jumpRequested = false;
    
    public InputHandler(Player player, CameraController cameraController, World world,
                       Runnable onBreakBlock, Runnable onPlaceBlock) {
        this.player = player;
        this.cameraController = cameraController;
        this.world = world;
        this.onBreakBlock = onBreakBlock;
        this.onPlaceBlock = onPlaceBlock;
        
        Gdx.input.setInputProcessor(this);
    }
    
    public void update(float delta) {
        if (!cameraController.isLocked()) return;
        
        // Handle movement
        float moveSpeed = Constants.PLAYER_SPEED * delta;
        
        if (keys[Input.Keys.W]) {
            cameraController.moveForward(moveSpeed);
        }
        if (keys[Input.Keys.S]) {
            cameraController.moveBackward(moveSpeed);
        }
        if (keys[Input.Keys.A]) {
            cameraController.moveLeft(moveSpeed);
        }
        if (keys[Input.Keys.D]) {
            cameraController.moveRight(moveSpeed);
        }
        
        // Handle jump
        if (jumpRequested) {
            jumpRequested = false;
            player.jump();
        }
        
        // Handle block selection
        if (keys[Input.Keys.NUM_1]) {
            selectedBlockType = Constants.BLOCK_GRASS;
            keys[Input.Keys.NUM_1] = false;
        }
        if (keys[Input.Keys.NUM_2]) {
            selectedBlockType = Constants.BLOCK_STONE;
            keys[Input.Keys.NUM_2] = false;
        }
        
        // Update camera
        cameraController.update(delta);
    }
    
    public String getSelectedBlockType() {
        return selectedBlockType;
    }
    
    @Override
    public boolean keyDown(int keycode) {
        if (keycode < 256) {
            keys[keycode] = true;
        }
        
        if (keycode == Input.Keys.SPACE) {
            jumpRequested = true;
            return true;
        }
        
        if (keycode == Input.Keys.NUM_1 || keycode == Input.Keys.NUM_2) {
            return true;
        }
        
        return false;
    }
    
    @Override
    public boolean keyUp(int keycode) {
        if (keycode < 256) {
            keys[keycode] = false;
        }
        return false;
    }
    
    @Override
    public boolean keyTyped(char character) {
        return false;
    }
    
    @Override
    public boolean touchDown(int screenX, int screenY, int pointer, int button) {
        if (!cameraController.isLocked()) {
            if (button == Input.Buttons.LEFT) {
                cameraController.lock();
                return true;
            }
        } else {
            if (button == Input.Buttons.LEFT) {
                onBreakBlock.run();
                return true;
            }
            if (button == Input.Buttons.RIGHT) {
                onPlaceBlock.run();
                return true;
            }
        }
        return false;
    }
    
    @Override
    public boolean touchUp(int screenX, int screenY, int pointer, int button) {
        return false;
    }
    
    @Override
    public boolean touchCancelled(int screenX, int screenY, int pointer, int button) {
        return false;
    }
    
    @Override
    public boolean touchDragged(int screenX, int screenY, int pointer) {
        return false;
    }
    
    @Override
    public boolean mouseMoved(int screenX, int screenY) {
        return false;
    }
    
    @Override
    public boolean scrolled(float amountX, float amountY) {
        return false;
    }
}
