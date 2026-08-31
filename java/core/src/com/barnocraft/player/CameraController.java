package com.barnocraft.player;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.graphics.Camera;
import com.badlogic.gdx.math.Vector2;
import com.badlogic.gdx.math.Vector3;
import com.barnocraft.Constants;

public class CameraController {
    private final Camera camera;
    private final Player player;
    private boolean locked = false;
    private float mouseSensitivity = 0.1f;
    private float pitch = 0f;
    private float yaw = 0f;
    private Vector2 lastMousePos = new Vector2();
    
    public CameraController(Camera camera, Player player) {
        this.camera = camera;
        this.player = player;
        this.camera.position.set(player.getEyePosition());
        this.camera.direction.set(0, 0, -1);
        this.camera.up.set(0, 1, 0);
    }
    
    public void lock() {
        locked = true;
        lastMousePos.set(Gdx.input.getX(), Gdx.input.getY());
        Gdx.input.setCursorCatched(true);
    }
    
    public void unlock() {
        locked = false;
        Gdx.input.setCursorCatched(false);
    }
    
    public boolean isLocked() {
        return locked;
    }
    
    public void update(float delta) {
        if (!locked) return;
        
        // Mouse look
        float mouseX = Gdx.input.getX();
        float mouseY = Gdx.input.getY();
        
        float deltaX = mouseX - lastMousePos.x;
        float deltaY = mouseY - lastMousePos.y;
        
        yaw -= deltaX * mouseSensitivity;
        pitch -= deltaY * mouseSensitivity;
        
        // Clamp pitch to avoid over-rotation
        pitch = Math.max(-89f, Math.min(89f, pitch));
        
        lastMousePos.set(mouseX, mouseY);
        
        // Update camera rotation
        camera.direction.set(
            (float) (Math.cos(Math.toRadians(yaw)) * Math.cos(Math.toRadians(pitch))),
            (float) Math.sin(Math.toRadians(pitch)),
            (float) (Math.sin(Math.toRadians(yaw)) * Math.cos(Math.toRadians(pitch)))
        ).nor();
        
        // Update camera position
        camera.position.set(player.getEyePosition());
        camera.update();
    }
    
    public Vector3 getDirection() {
        return new Vector3(camera.direction);
    }
    
    public void moveForward(float amount) {
        Vector3 forward = new Vector3(camera.direction);
        forward.y = 0;
        forward.nor();
        player.position.add(forward.tmp().scl(amount));
    }
    
    public void moveBackward(float amount) {
        Vector3 forward = new Vector3(camera.direction);
        forward.y = 0;
        forward.nor();
        player.position.add(forward.tmp().scl(-amount));
    }
    
    public void moveLeft(float amount) {
        Vector3 right = new Vector3();
        right.set(camera.direction).crs(camera.up).nor();
        player.position.add(right.tmp().scl(-amount));
    }
    
    public void moveRight(float amount) {
        Vector3 right = new Vector3();
        right.set(camera.direction).crs(camera.up).nor();
        player.position.add(right.tmp().scl(amount));
    }
}
