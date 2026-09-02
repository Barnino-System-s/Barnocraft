package com.barnocraft;

import com.jme3.app.SimpleApplication;
import com.jme3.asset.AssetManager;
import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.MouseInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.controls.MouseButtonTrigger;
import com.jme3.light.AmbientLight;
import com.jme3.light.DirectionalLight;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Vector3f;
import com.jme3.renderer.ViewPort;
import com.jme3.scene.Node;
import com.jme3.system.AppSettings;
import com.jme3.app.state.FlyCamAppState;

/**
 * Main class for Barnocraft game using jMonkeyEngine
 */
public class Main extends SimpleApplication {
    
    private World world;
    private Player player;
    private Textures textures;
    
    private BlockType selectedBlockType = BlockType.STONE;
    private boolean mouseLocked = true;
    
    public static void main(String[] args) {
        Main app = new Main();
        
        // Configure window
        AppSettings settings = new AppSettings(true);
        settings.setWidth(1280);
        settings.setHeight(720);
        settings.setTitle("Barnocraft - jMonkeyEngine");
        
        app.setSettings(settings);
        app.start();
    }
    
    @Override
    public void simpleInitApp() {
        // Set sky color to match the original game
        viewPort.setBackgroundColor(new ColorRGBA(0.529f, 0.808f, 0.922f, 1.0f));
        
        // Initialize asset manager
        AssetManager assetManager = getAssetManager();
        
        // Disable default flycam
        stateManager.detach(stateManager.getState(FlyCamAppState.class));
        
        // Create textures manager
        textures = new Textures(assetManager);
        
        // Create world
        world = new World(rootNode, textures);
        
        // Setup lighting
        setupLighting();
        
        // Setup camera
        setupCamera();
        
        // Create player
        InputManager inputManager = getInputManager();
        player = new Player(rootNode, cam, world, inputManager);
        
        // Setup additional controls for block selection
        setupBlockSelectionControls(inputManager);
        
        // Hide cursor for first-person view
        getInputManager().setCursorVisible(false);
        
        // Lock mouse to window
        lockMouse();
        
        System.out.println("Barnocraft initialized with jMonkeyEngine");
        System.out.println("Controls: WASD - Move, Space - Jump, LMB - Break block, RMB - Place block");
        System.out.println("1 - Select Grass, 2 - Select Stone, ESC - Toggle mouse");
    }
    
    private void setupLighting() {
        // Ambient light
        AmbientLight ambientLight = new AmbientLight();
        ambientLight.setColor(ColorRGBA.White.mult(0.7f));
        rootNode.addLight(ambientLight);
        
        // Directional light (sun)
        DirectionalLight sun = new DirectionalLight();
        sun.setColor(ColorRGBA.White.mult(0.8f));
        sun.setDirection(new Vector3f(-0.6f, -1f, -0.4f).normalizeLocal());
        rootNode.addLight(sun);
    }
    
    private void setupCamera() {
        // Set camera location to player start position
        cam.setLocation(new Vector3f(World.WORLD_WIDTH / 2f, 7.62f, World.WORLD_DEPTH / 2f));
        cam.lookAtDirection(new Vector3f(0, 0, 1), Vector3f.UNIT_Y);
    }
    
    private void setupBlockSelectionControls(InputManager inputManager) {
        inputManager.addMapping("SelectGrass", new KeyTrigger(KeyInput.KEY_1));
        inputManager.addMapping("SelectStone", new KeyTrigger(KeyInput.KEY_2));
        inputManager.addMapping("ToggleMouse", new KeyTrigger(KeyInput.KEY_ESCAPE));
        
        inputManager.addListener(new ActionListener() {
            @Override
            public void onAction(String name, boolean isPressed, float tpf) {
                if (!isPressed) return;
                
                switch (name) {
                    case "SelectGrass":
                        selectedBlockType = BlockType.GRASS;
                        System.out.println("Selected: Grass");
                        break;
                    case "SelectStone":
                        selectedBlockType = BlockType.STONE;
                        System.out.println("Selected: Stone");
                        break;
                    case "ToggleMouse":
                        mouseLocked = !mouseLocked;
                        if (mouseLocked) {
                            lockMouse();
                        } else {
                            unlockMouse();
                        }
                        break;
                }
            }
        }, "SelectGrass", "SelectStone", "ToggleMouse");
    }
    
    private void lockMouse() {
        getInputManager().setCursorVisible(false);
        System.out.println("Mouse locked");
    }
    
    private void unlockMouse() {
        getInputManager().setCursorVisible(true);
        System.out.println("Mouse unlocked");
    }
    
    @Override
    public void simpleUpdate(float tpf) {
        // Update player
        if (player != null) {
            player.update(tpf);
        }
    }
    
    @Override
    public void simpleRender(com.jme3.renderer.RenderManager rm) {
        // Default rendering
    }
    
    @Override
    public void destroy() {
        // Cleanup resources
        if (textures != null) {
            textures.cleanup();
        }
        if (world != null) {
            world.cleanup();
        }
        if (player != null) {
            player.cleanup();
        }
        super.destroy();
    }
}
