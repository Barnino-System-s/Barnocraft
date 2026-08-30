class_name WorldConstants

extends RefCounted


# Global constants for world configuration

# Chunk dimensions
const CHUNK_SIZE_X: int = 16
const CHUNK_SIZE_Z: int = 16
const CHUNK_SIZE_Y: int = 128

# World height
const WORLD_HEIGHT: int = 128

# Chunk loading distances
const CHUNK_LOAD_DISTANCE: int = 8
const CHUNK_UNLOAD_DISTANCE: int = 10

# Generation settings
const MIN_GENERATION_HEIGHT: int = 0
const MAX_GENERATION_HEIGHT: int = WORLD_HEIGHT - 1

# Total blocks in a chunk
const CHUNK_VOLUME: int = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z

# Layer masks
const BLOCK_LAYER_MASK: int = 1
