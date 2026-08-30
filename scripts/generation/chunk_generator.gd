class_name ChunkGenerator

extends RefCounted


# Generates chunk data procedurally

# Configuration
const GRASS_LAYER_HEIGHT: int = 1
const DIRT_LAYERS: int = 4
const COBBLESTONE_LAYERS: int = 10
const COBBLESTONE_2_LAYERS: int = 20
const SUPERBLOCK_LAYERS: int = 10

const BASE_HEIGHT: int = 64
const HEIGHT_VARIATION: int = 8

# Random number generator
var random: RandomNumberGenerator

# World seed
var seed: String = ""


func _init(p_seed: String):
	seed = p_seed
	random = RandomNumberGenerator.new()
	random.seed = seed.hash()


# Generate a chunk
func generate_chunk(coord: ChunkCoordinates, world: World) -> Chunk:
	var chunk = Chunk.new(coord, world)
	
	# Generate each column in the chunk
	for local_x in range(WorldConstants.CHUNK_SIZE_X):
		for local_z in range(WorldConstants.CHUNK_SIZE_Z):
			var world_x = coord.x * WorldConstants.CHUNK_SIZE_X + local_x
			var world_z = coord.z * WorldConstants.CHUNK_SIZE_Z + local_z
			
			# Generate height for this position
			var surface_height = _generate_height(world_x, world_z)
			
			# Generate column of blocks
			_generate_column(chunk, local_x, local_z, surface_height)
	
	return chunk


func _generate_height(world_x: int, world_z: int) -> int:
	# Deterministic height generation based on coordinates and seed
	var hash = _get_deterministic_hash(world_x, world_z)
	
	# Use hash to seed a temporary RNG
	var temp_random = RandomNumberGenerator.new()
	temp_random.seed = hash
	
	# Base height + variation
	var height = BASE_HEIGHT + temp_random.randi_range(-HEIGHT_VARIATION, HEIGHT_VARIATION)
	
	# Clamp to world bounds
	return clamp(height, WorldConstants.MIN_GENERATION_HEIGHT, WorldConstants.MAX_GENERATION_HEIGHT)


func _get_deterministic_hash(x: int, z: int) -> int:
	# Simple deterministic hash
	var hash = seed.hash()
	hash = (hash * 2654435761) ^ x
	hash = (hash * 2654435761) ^ z
	return hash


func _generate_column(chunk: Chunk, local_x: int, local_z: int, surface_height: int):
	for local_y in range(WorldConstants.CHUNK_SIZE_Y):
		var world_y = local_y
		
		if world_y > surface_height:
			# Above surface - air
			chunk.set_block(local_x, local_y, local_z, BlockType.AIR)
		elif world_y == surface_height:
			# Surface - grass
			chunk.set_block(local_x, local_y, local_z, BlockType.GRASS_BLOCK)
		elif world_y > surface_height - GRASS_LAYER_HEIGHT - DIRT_LAYERS:
			# Dirt layers
			chunk.set_block(local_x, local_y, local_z, BlockType.DIRT)
		elif world_y > surface_height - GRASS_LAYER_HEIGHT - DIRT_LAYERS - COBBLESTONE_LAYERS:
			# Cobblestone layers
			chunk.set_block(local_x, local_y, local_z, BlockType.COBBLESTONE)
		elif world_y > surface_height - GRASS_LAYER_HEIGHT - DIRT_LAYERS - COBBLESTONE_LAYERS - COBBLESTONE_2_LAYERS:
			# Cobblestone 2 layers
			chunk.set_block(local_x, local_y, local_z, BlockType.COBBLESTONE_2)
		else:
			# Bottom - superblock
			chunk.set_block(local_x, local_y, local_z, BlockType.SUPERBLOCK)


func cleanup():
	if random != null:
		random = null
