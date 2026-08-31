package com.barnocraft.player;

import com.badlogic.gdx.math.Vector3;
import com.barnocraft.Constants;

public class Player {
    public Vector3 position = new Vector3();
    public Vector3 velocity = new Vector3();
    public boolean onGround = true;
    public float width = Constants.PLAYER_WIDTH;
    public float height = Constants.PLAYER_HEIGHT;
    public float eyeHeight = Constants.PLAYER_EYE_HEIGHT;
    
    public Player() {
        position.set(
            Constants.WORLD_WIDTH / 2f,
            6.001f,
            Constants.WORLD_DEPTH / 2f
        );
    }
    
    public void update(float delta) {
        // Gravity
        velocity.y += Constants.GRAVITY * delta;
        
        // Update position
        position.y += velocity.y * delta;
    }
    
    public Vector3 getEyePosition() {
        return new Vector3(position.x, position.y + eyeHeight, position.z);
    }
    
    public void setPosition(float x, float y, float z) {
        position.set(x, y, z);
    }
    
    public void jump() {
        if (onGround) {
            velocity.y = Constants.JUMP_SPEED;
            onGround = false;
        }
    }
}
