#pragma once

namespace Barnocraft::World {

    // Chunk dimensions
    constexpr int CHUNK_SIZE_X = 16;
    constexpr int CHUNK_SIZE_Z = 16;
    constexpr int CHUNK_SIZE_Y = 128;
    
    // World height
    constexpr int WORLD_HEIGHT = 128;
    
    // Chunk loading distances
    constexpr int CHUNK_LOAD_DISTANCE = 8;
    constexpr int CHUNK_UNLOAD_DISTANCE = 10;
    
    // Generation settings
    constexpr int MIN_GENERATION_HEIGHT = 0;
    constexpr int MAX_GENERATION_HEIGHT = WORLD_HEIGHT - 1;
    
    // Total blocks in a chunk
    constexpr int CHUNK_VOLUME = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;
    
    // Base height for terrain generation
    constexpr int BASE_HEIGHT = 64;
    constexpr int HEIGHT_VARIATION = 8;
    
    // Layer thickness for terrain
    constexpr int GRASS_LAYER_HEIGHT = 1;
    constexpr int DIRT_LAYERS = 4;
    constexpr int COBBLESTONE_LAYERS = 10;
    constexpr int COBBLESTONE_2_LAYERS = 20;
    constexpr int SUPERBLOCK_LAYERS = WORLD_HEIGHT - BASE_HEIGHT - HEIGHT_VARIATION - 
                                       GRASS_LAYER_HEIGHT - DIRT_LAYERS - COBBLESTONE_LAYERS - COBBLESTONE_2_LAYERS;

} // namespace Barnocraft::World
