package com.barnocraft;

import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.MouseInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.AnalogListener;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.controls.MouseAxisTrigger;
import com.jme3.input.controls.MouseButtonTrigger;
import com.jme3.math.FastMath;
import com.jme3.math.Quaternion;
import com.jme3.math.Vector3f;
import com.jme3.renderer.Camera;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import com.jme3.input.RawInputListener;
import com.jme3.input.event.JoyAxisEvent;
import com.jme3.input.event.JoyButtonEvent;
import com.jme3.input.event.KeyInputEvent;
import com.jme3.input.event.MouseButtonEvent;
import com.jme3.input.event.MouseMotionEvent;

/**
 * Manages player movement, collision, and camera
 */
public class Player implements ActionListener, AnalogListener, RawInputListener {
    
    private static final float PLAYER_WIDTH = 0.6f;
    private static final float PLAYER_HEIGHT = 1.8f;
    private static final float EYE_HEIGHT = 1.62f;
    private static final float WALK_SPEED = 4.0f;
    private static final float JUMP_SPEED = 8.0f;
    private static final float GRAVITY = -25.0f;
    
    private final Node playerNode;
    private final Camera camera;
    private final World world;
    
    private Vector3f position;
    private Vector3f velocity;
    private boolean onGround;
    private boolean jumpRequested;
    
    private float mouseSensitivity = 0.002f;
    private float pitch = 0f;
    private float yaw = 0f;
    private float mouseX = 0f;
    private float mouseY = 0f;
    private boolean mouseLocked = true;
    
    // Movement state
    private boolean moveForward = false;
    private boolean moveBackward = false;
    private boolean moveLeft = false;
    private boolean moveRight = false;
    
    public Player(Node rootNode, Camera camera, World world, InputManager inputManager) {
        this.playerNode = new Node("PlayerNode");
        rootNode.attachChild(playerNode);
        this.camera = camera;
        this.world = world;
        
        // Initial position (center of the world, slightly above ground)
        this.position = new Vector3f(World.WORLD_WIDTH / 2f, 6.001f, World.WORLD_DEPTH / 2f);
        this.velocity = new Vector3f(0, 0, 0);
        this.onGround = true;
        this.jumpRequested = false;
        
        // Initialize camera rotation
        camera.setRotation(new Quaternion().fromAngles(0, 0, 0));
        
        setupControls(inputManager);
    }
    
    private void setupControls(InputManager inputManager) {
        // Keyboard controls
        inputManager.addMapping("Forward", new KeyTrigger(KeyInput.KEY_W));
        inputManager.addMapping("Backward", new KeyTrigger(KeyInput.KEY_S));
        inputManager.addMapping("Left", new KeyTrigger(KeyInput.KEY_A));
        inputManager.addMapping("Right", new KeyTrigger(KeyInput.KEY_D));
        inputManager.addMapping("Jump", new KeyTrigger(KeyInput.KEY_SPACE));
        inputManager.addMapping("BreakBlock", new MouseButtonTrigger(MouseInput.BUTTON_LEFT));
        inputManager.addMapping("PlaceBlock", new MouseButtonTrigger(MouseInput.BUTTON_RIGHT));
        inputManager.addMapping("SelectGrass", new KeyTrigger(KeyInput.KEY_1));
        inputManager.addMapping("SelectStone", new KeyTrigger(KeyInput.KEY_2));
        
        // Mouse look
        inputManager.addMapping("MouseLeft", new MouseAxisTrigger(MouseInput.AXIS_X, true));
        inputManager.addMapping("MouseRight", new MouseAxisTrigger(MouseInput.AXIS_X, false));
        inputManager.addMapping("MouseUp", new MouseAxisTrigger(MouseInput.AXIS_Y, false));
        inputManager.addMapping("MouseDown", new MouseAxisTrigger(MouseInput.AXIS_Y, true));
        
        inputManager.addListener(this, "Forward", "Backward", "Left", "Right", "Jump", 
                                  "BreakBlock", "PlaceBlock", "SelectGrass", "SelectStone");
        inputManager.addListener(this, "MouseLeft", "MouseRight", "MouseUp", "MouseDown");
        
        // Add raw input listener for mouse motion
        inputManager.addRawInputListener(this);
    }
    
    @Override
    public void onAction(String name, boolean isPressed, float tpf) {
        switch (name) {
            case "Forward":
                moveForward = isPressed;
                break;
            case "Backward":
                moveBackward = isPressed;
                break;
            case "Left":
                moveLeft = isPressed;
                break;
            case "Right":
                moveRight = isPressed;
                break;
            case "Jump":
                if (isPressed && !jumpRequested) {
                    jumpRequested = true;
                }
                break;
            case "BreakBlock":
                if (isPressed) {
                    breakBlock();
                }
                break;
            case "PlaceBlock":
                if (isPressed) {
                    placeBlock();
                }
                break;
            case "SelectGrass":
                if (isPressed) {
                    // Selection handled in Main class
                }
                break;
            case "SelectStone":
                if (isPressed) {
                    // Selection handled in Main class
                }
                break;
        }
    }
    
    @Override
    public void onAnalog(String name, float value, float tpf) {
        // Handle mouse look
        if (name.equals("MouseLeft")) {
            yaw -= value * mouseSensitivity * 100f;
        } else if (name.equals("MouseRight")) {
            yaw += value * mouseSensitivity * 100f;
        } else if (name.equals("MouseUp")) {
            pitch -= value * mouseSensitivity * 100f;
        } else if (name.equals("MouseDown")) {
            pitch += value * mouseSensitivity * 100f;
        }
        
        // Clamp pitch to prevent over-rotation
        pitch = FastMath.clamp(pitch, -FastMath.PI / 2f + 0.01f, FastMath.PI / 2f - 0.01f);
    }
    
    public void update(float tpf) {
        updateJump();
        updateVertical(tpf);
        updateHorizontal(tpf);
        updateCameraRotation();
        updateCamera();
    }
    
    private void updateCameraRotation() {
        // Apply rotation to camera
        Quaternion rotation = new Quaternion();
        rotation.fromAngles(pitch, yaw, 0);
        camera.setRotation(rotation);
    }
    
    private void updateJump() {
        if (!jumpRequested) return;
        jumpRequested = false;
        if (!onGround) return;
        
        velocity.y = JUMP_SPEED;
        onGround = false;
    }
    
    private void updateVertical(float tpf) {
        float oldY = position.y;
        
        // Apply gravity
        velocity.y += GRAVITY * tpf;
        float newY = position.y + velocity.y * tpf;
        
        if (velocity.y <= 0) {
            // Falling - check for ground collision
            float halfWidth = PLAYER_WIDTH / 2f;
            int minX = (int) Math.floor(position.x - halfWidth);
            int maxX = (int) Math.floor(position.x + halfWidth - 0.000001f);
            int minZ = (int) Math.floor(position.z - halfWidth);
            int maxZ = (int) Math.floor(position.z + halfWidth - 0.000001f);
            
            float floorY = Float.NEGATIVE_INFINITY;
            
            for (int x = minX; x <= maxX; x++) {
                for (int z = minZ; z <= maxZ; z++) {
                    if (x < 0 || x >= World.WORLD_WIDTH || z < 0 || z >= World.WORLD_DEPTH) continue;
                    
                    int firstY = (int) Math.floor(newY);
                    int lastY = (int) Math.floor(oldY);
                    
                    for (int y = firstY; y <= lastY; y++) {
                        if (!world.isSolid(x, y, z)) continue;
                        float top = y + 1;
                        if (oldY >= top && newY <= top) {
                            if (top > floorY) floorY = top;
                        }
                    }
                }
            }
            
            if (floorY != Float.NEGATIVE_INFINITY) {
                position.y = floorY;
                velocity.y = 0;
                onGround = true;
                return;
            }
        }
        
        if (velocity.y > 0) {
            // Rising - check for ceiling collision
            float oldTop = oldY + PLAYER_HEIGHT;
            float newTop = newY + PLAYER_HEIGHT;
            float halfWidth = PLAYER_WIDTH / 2f;
            
            int minX = (int) Math.floor(position.x - halfWidth);
            int maxX = (int) Math.floor(position.x + halfWidth - 0.000001f);
            int minZ = (int) Math.floor(position.z - halfWidth);
            int maxZ = (int) Math.floor(position.z + halfWidth - 0.000001f);
            
            int firstY = (int) Math.floor(oldTop);
            int lastY = (int) Math.floor(newTop - 0.000001f);
            
            float ceilingY = Float.POSITIVE_INFINITY;
            
            for (int x = minX; x <= maxX; x++) {
                for (int z = minZ; z <= maxZ; z++) {
                    for (int y = firstY; y <= lastY; y++) {
                        if (!world.isSolid(x, y, z)) continue;
                        if (y < ceilingY) ceilingY = y;
                    }
                }
            }
            
            if (ceilingY != Float.POSITIVE_INFINITY) {
                position.y = ceilingY - PLAYER_HEIGHT;
                velocity.y = 0;
                onGround = false;
                return;
            }
        }
        
        position.y = newY;
        onGround = false;
    }
    
    private void updateHorizontal(float tpf) {
        // Calculate movement direction based on camera rotation
        Vector3f forward = camera.getDirection().clone();
        forward.y = 0;
        if (forward.lengthSquared() > 0) forward.normalizeLocal();
        
        Vector3f right = camera.getLeft().clone();
        right.y = 0;
        if (right.lengthSquared() > 0) right.normalizeLocal();
        
        Vector3f moveDirection = new Vector3f(0, 0, 0);
        
        if (moveForward) moveDirection.addLocal(forward);
        if (moveBackward) moveDirection.addLocal(forward.negate());
        if (moveLeft) moveDirection.addLocal(right.negate());
        if (moveRight) moveDirection.addLocal(right);
        
        if (moveDirection.lengthSquared() > 0) {
            moveDirection.normalizeLocal();
        }
        
        float distance = WALK_SPEED * tpf;
        float newX = position.x + moveDirection.x * distance;
        float newZ = position.z + moveDirection.z * distance;
        
        if (!collides(newX, position.y, position.z)) {
            position.x = newX;
        }
        if (!collides(position.x, position.y, newZ)) {
            position.z = newZ;
        }
        
        // Clamp to world boundaries
        float halfWidth = PLAYER_WIDTH / 2f;
        position.x = Math.max(halfWidth, Math.min(World.WORLD_WIDTH - halfWidth, position.x));
        position.z = Math.max(halfWidth, Math.min(World.WORLD_DEPTH - halfWidth, position.z));
    }
    
    private boolean collides(float px, float py, float pz) {
        float halfWidth = PLAYER_WIDTH / 2f;
        float minX = px - halfWidth;
        float maxX = px + halfWidth;
        float minY = py;
        float maxY = py + PLAYER_HEIGHT;
        float minZ = pz - halfWidth;
        float maxZ = pz + halfWidth;
        
        int minBlockX = (int) Math.floor(minX);
        int maxBlockX = (int) Math.floor(maxX - 0.000001f);
        int minBlockY = (int) Math.floor(minY);
        int maxBlockY = (int) Math.floor(maxY - 0.000001f);
        int minBlockZ = (int) Math.floor(minZ);
        int maxBlockZ = (int) Math.floor(maxZ - 0.000001f);
        
        for (int y = minBlockY; y <= maxBlockY; y++) {
            for (int x = minBlockX; x <= maxBlockX; x++) {
                for (int z = minBlockZ; z <= maxBlockZ; z++) {
                    if (world.isSolid(x, y, z)) {
                        return true;
                    }
                }
            }
        }
        
        return false;
    }
    
    private void updateCamera() {
        camera.setLocation(position.add(new Vector3f(0, EYE_HEIGHT, 0)));
    }
    
    private void breakBlock() {
        // Implement raycasting to find the block the player is looking at
        // For now, simple implementation
        Vector3f cameraDir = camera.getDirection();
        Vector3f cameraPos = camera.getLocation();
        
        // Simple raycast: check blocks in front of the player
        for (int distance = 1; distance <= 10; distance++) {
            int x = (int) Math.floor(cameraPos.x + cameraDir.x * distance);
            int y = (int) Math.floor(cameraPos.y + cameraDir.y * distance);
            int z = (int) Math.floor(cameraPos.z + cameraDir.z * distance);
            
            if (x < 0 || x >= World.WORLD_WIDTH || y < 0 || y >= World.WORLD_HEIGHT || z < 0 || z >= World.WORLD_DEPTH) {
                continue;
            }
            
            if (world.isSolid(x, y, z)) {
                // Check if the block is not intersecting with the player
                if (!blockIntersectsPlayer(x, y, z)) {
                    world.setBlock(x, y, z, BlockType.AIR);
                }
                return;
            }
        }
    }
    
    private void placeBlock() {
        // Implement placement at the adjacent position to the looked-at block
        Vector3f cameraDir = camera.getDirection();
        Vector3f cameraPos = camera.getLocation();
        
        // Find the first solid block
        int targetX = -1, targetY = -1, targetZ = -1;
        for (int distance = 1; distance <= 10; distance++) {
            int x = (int) Math.floor(cameraPos.x + cameraDir.x * distance);
            int y = (int) Math.floor(cameraPos.y + cameraDir.y * distance);
            int z = (int) Math.floor(cameraPos.z + cameraDir.z * distance);
            
            if (x < 0 || x >= World.WORLD_WIDTH || y < 0 || y >= World.WORLD_HEIGHT || z < 0 || z >= World.WORLD_DEPTH) {
                continue;
            }
            
            if (world.isSolid(x, y, z)) {
                targetX = x;
                targetY = y;
                targetZ = z;
                break;
            }
        }
        
        if (targetX != -1) {
            // Calculate adjacent position based on the normal
            // Simple implementation: try to place in front of the block
            Vector3f normal = estimateNormal(targetX, targetY, targetZ);
            int newX = targetX + (int) Math.round(normal.x);
            int newY = targetY + (int) Math.round(normal.y);
            int newZ = targetZ + (int) Math.round(normal.z);
            
            if (newX >= 0 && newX < World.WORLD_WIDTH && 
                newY >= 0 && newY < World.WORLD_HEIGHT && 
                newZ >= 0 && newZ < World.WORLD_DEPTH) {
                
                if (!world.isSolid(newX, newY, newZ) && !blockIntersectsPlayer(newX, newY, newZ)) {
                    // Place the selected block type (this will be set from Main class)
                    // For now, place stone
                    world.setBlock(newX, newY, newZ, BlockType.STONE);
                }
            }
        }
    }
    
    private Vector3f estimateNormal(int x, int y, int z) {
        // Simple normal estimation based on which face is closest to the camera
        Vector3f cameraPos = camera.getLocation();
        Vector3f blockCenter = new Vector3f(x + 0.5f, y + 0.5f, z + 0.5f);
        Vector3f toBlock = blockCenter.subtract(cameraPos);
        
        // Find the dominant component
        float absX = Math.abs(toBlock.x);
        float absY = Math.abs(toBlock.y);
        float absZ = Math.abs(toBlock.z);
        
        if (absX >= absY && absX >= absZ) {
            return new Vector3f(toBlock.x > 0 ? -1 : 1, 0, 0);
        } else if (absY >= absX && absY >= absZ) {
            return new Vector3f(0, toBlock.y > 0 ? -1 : 1, 0);
        } else {
            return new Vector3f(0, 0, toBlock.z > 0 ? -1 : 1);
        }
    }
    
    private boolean blockIntersectsPlayer(int bx, int by, int bz) {
        float halfWidth = PLAYER_WIDTH / 2f;
        float playerMinX = position.x - halfWidth;
        float playerMaxX = position.x + halfWidth;
        float playerMinY = position.y;
        float playerMaxY = position.y + PLAYER_HEIGHT;
        float playerMinZ = position.z - halfWidth;
        float playerMaxZ = position.z + halfWidth;
        
        return (playerMinX < bx + 1 && playerMaxX > bx &&
                playerMinY < by + 1 && playerMaxY > by &&
                playerMinZ < bz + 1 && playerMaxZ > bz);
    }
    
    public Vector3f getPosition() {
        return position.clone();
    }
    
    public void setPosition(Vector3f position) {
        this.position = position.clone();
    }
    
    // RawInputListener methods for mouse control
    @Override
    public void beginInput() {}
    
    @Override
    public void endInput() {}
    
    @Override
    public void onJoyAxisEvent(JoyAxisEvent evt) {}
    
    @Override
    public void onJoyButtonEvent(JoyButtonEvent evt) {}
    
    @Override
    public void onMouseMotionEvent(MouseMotionEvent evt) {
        if (!mouseLocked) return;
        
        float dx = evt.getDeltaX();
        float dy = evt.getDeltaY();
        
        yaw -= dx * mouseSensitivity;
        pitch += dy * mouseSensitivity;
        
        // Clamp pitch to prevent over-rotation
        pitch = FastMath.clamp(pitch, -FastMath.PI / 2f + 0.01f, FastMath.PI / 2f - 0.01f);
    }
    
    @Override
    public void onMouseButtonEvent(MouseButtonEvent evt) {
        // Handle mouse button events
    }
    
    @Override
    public void onKeyEvent(KeyInputEvent evt) {
        // Handle key events
    }
    
    public void setMouseLocked(boolean locked) {
        this.mouseLocked = locked;
    }
    
    public void setSelectedBlockType(BlockType blockType) {
        // This will be used to set the block type for placement
        // Implementation in Main class will handle this
    }
    
    public void cleanup() {
        playerNode.removeFromParent();
    }
}
