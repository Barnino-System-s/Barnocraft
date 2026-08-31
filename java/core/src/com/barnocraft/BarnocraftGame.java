package com.barnocraft;

import com.badlogic.gdx.ApplicationAdapter;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Camera;
import com.badlogic.gdx.graphics.PerspectiveCamera;
import com.badlogic.gdx.graphics.g3d.environment.DirectionalLight;
import com.badlogic.gdx.math.Vector3;
import com.barnocraft.blocks.BlockRegistry;
import com.barnocraft.input.InputHandler;
import com.barnocraft.player.CameraController;
import com.barnocraft.player.Player;
import com.barnocraft.raycast.RaycastResult;
import com.barnocraft.raycast.Raycaster;
import com.barnocraft.rendering.HUD;
import com.barnocraft.rendering.WorldRenderer;
import com.barnocraft.world.World;

public class BarnocraftGame extends ApplicationAdapter {
    private PerspectiveCamera camera;
    private CameraController cameraController;
    private Player player;
    private World world;
    private BlockRegistry blockRegistry;
    private WorldRenderer worldRenderer;
    private HUD hud;
    private InputHandler inputHandler;
    private Raycaster raycaster;
    
    @Override
    public void create() {
        // Initialize block registry and load textures
        blockRegistry = new BlockRegistry();
        blockRegistry.loadTextures();
        
        // Create world
        world = new World(blockRegistry);
        
        // Create player
        player = new Player();
        
        // Create camera
        camera = new PerspectiveCamera(
            Constants.FOV,
            Gdx.graphics.getWidth(),
            Gdx.graphics.getHeight()
        );
        camera.near = 0.05f;
        camera.far = 500f;
        camera.position.set(player.getEyePosition());
        camera.update();
        
        // Create camera controller
        cameraController = new CameraController(camera, player);
        
        // Create raycaster
        raycaster = new Raycaster(world);
        
        // Create world renderer
        worldRenderer = new WorldRenderer(world);
        
        // Create HUD
        hud = new HUD();
        
        // Create input handler
        inputHandler = new InputHandler(
            player,
            cameraController,
            world,
            this::breakBlock,
            this::placeBlock
        );
        
        // Build initial chunk meshes
        rebuildAllChunks();
        
        // Show initial info
        hud.showInfo(true);
        hud.showHotbar(true);
    }
    
    private void rebuildAllChunks() {
        for (int cx = 0; cx < Constants.CHUNKS_X; cx++) {
            for (int cz = 0; cz < Constants.CHUNKS_Z; cz++) {
                // Rebuild chunk mesh
                // This will be implemented properly in WorldRenderer
            }
        }
    }
    
    private void breakBlock() {
        Vector3 eyePos = player.getEyePosition();
        Vector3 direction = cameraController.getDirection();
        
        // Offset origin slightly to avoid hitting player's own position
        Vector3 origin = new Vector3(eyePos).add(direction.tmp().scl(0.01f));
        
        RaycastResult result = raycaster.raycast(origin, direction, Constants.VIEW_DISTANCE);
        if (result != null) {
            int x = result.x;
            int y = result.y;
            int z = result.z;
            
            // Check if block intersects with player
            if (blockIntersectsPlayer(x, y, z)) return;
            
            // Break the block
            world.setBlock(x, y, z, null);
            
            // Rebuild the chunk mesh
            int chunkX = x / Constants.CHUNK_SIZE;
            int chunkZ = z / Constants.CHUNK_SIZE;
            // worldRenderer.rebuildChunkMesh(world.getChunk(chunkX, chunkZ), ...);
        }
    }
    
    private void placeBlock() {
        Vector3 eyePos = player.getEyePosition();
        Vector3 direction = cameraController.getDirection();
        
        Vector3 origin = new Vector3(eyePos).add(direction.tmp().scl(0.01f));
        
        RaycastResult result = raycaster.raycast(origin, direction, Constants.VIEW_DISTANCE);
        if (result != null) {
            Vector3 adjacent = result.getAdjacentBlockPosition();
            int x = (int) adjacent.x;
            int y = (int) adjacent.y;
            int z = (int) adjacent.z;
            
            // Check bounds
            if (!world.isInBounds(x, y, z)) return;
            
            // Check if block is air
            if (world.isSolid(x, y, z)) return;
            
            // Check if block intersects with player
            if (blockIntersectsPlayer(x, y, z)) return;
            
            // Place the block
            String blockType = inputHandler.getSelectedBlockType();
            // world.setBlock(x, y, z, new Block(blockRegistry.getBlockType(blockType)));
            
            // Rebuild the chunk mesh
            int chunkX = x / Constants.CHUNK_SIZE;
            int chunkZ = z / Constants.CHUNK_SIZE;
            // worldRenderer.rebuildChunkMesh(world.getChunk(chunkX, chunkZ), ...);
        }
    }
    
    private boolean blockIntersectsPlayer(int bx, int by, int bz) {
        float halfWidth = player.width / 2f;
        float playerMinX = player.position.x - halfWidth;
        float playerMaxX = player.position.x + halfWidth;
        float playerMinY = player.position.y;
        float playerMaxY = player.position.y + player.height;
        float playerMinZ = player.position.z - halfWidth;
        float playerMaxZ = player.position.z + halfWidth;
        
        return (playerMinX < bx + 1 && playerMaxX > bx &&
                playerMinY < by + 1 && playerMaxY > by &&
                playerMinZ < bz + 1 && playerMaxZ > bz);
    }
    
    private void updateHighlight() {
        if (!cameraController.isLocked()) {
            worldRenderer.hideHighlight();
            return;
        }
        
        Vector3 eyePos = player.getEyePosition();
        Vector3 direction = cameraController.getDirection();
        Vector3 origin = new Vector3(eyePos).add(direction.tmp().scl(0.01f));
        
        RaycastResult result = raycaster.raycast(origin, direction, Constants.VIEW_DISTANCE);
        if (result != null) {
            worldRenderer.updateHighlight(result.x, result.y, result.z);
        } else {
            worldRenderer.hideHighlight();
        }
    }
    
    @Override
    public void render() {
        // Update player
        player.update(Gdx.graphics.getDeltaTime());
        
        // Update camera controller
        cameraController.update(Gdx.graphics.getDeltaTime());
        
        // Update input
        inputHandler.update(Gdx.graphics.getDeltaTime());
        
        // Update camera position
        camera.position.set(player.getEyePosition());
        camera.update();
        
        // Update highlight
        updateHighlight();
        
        // Render world
        worldRenderer.render(camera);
        
        // Render HUD
        hud.render();
    }
    
    @Override
    public void resize(int width, int height) {
        camera.viewportWidth = width;
        camera.viewportHeight = height;
        camera.update();
        hud.resize(width, height);
    }
    
    @Override
    public void dispose() {
        worldRenderer.dispose();
        hud.dispose();
        blockRegistry.dispose();
        world.dispose();
    }
}
