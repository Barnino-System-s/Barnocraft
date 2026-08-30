class_name ChunkManager

extends RefCounted


# Manages loading, unloading, and accessing chunks

signal chunk_loaded(chunk: Chunk)
signal chunk_unloaded(chunk_coord: ChunkCoordinates)


# Reference to world
var world: World

# Chunk generator
var chunk_generator: ChunkGenerator

# Loaded chunks
var loaded_chunks: Dictionary = {}

# Chunks currently being generated
var generating_chunks: Dictionary = {}

# Queue for chunk generation
var generation_queue: List[ChunkCoordinates] = []

# Last known player position
var last_player_position: Vector3 = Vector3.ZERO

# Maximum chunks to generate per frame
const MAX_GENERATIONS_PER_FRAME: int = 4


func _init(p_world: World, p_generator: ChunkGenerator):
	world = p_world
	chunk_generator = p_generator


func update_player_position(p_position: Vector3):
	last_player_position = p_position


func update():
	_update_chunk_loading()
	_process_generation_queue()


func _update_chunk_loading():
	var player_chunk = ChunkCoordinates.from_world_position(last_player_position.x, last_player_position.z)
	
	# Determine required chunks
	var required_chunks: Dictionary = {}
	
	for x in range(-WorldConstants.CHUNK_LOAD_DISTANCE, WorldConstants.CHUNK_LOAD_DISTANCE + 1):
		for z in range(-WorldConstants.CHUNK_LOAD_DISTANCE, WorldConstants.CHUNK_LOAD_DISTANCE + 1):
			var coord = ChunkCoordinates.new(player_chunk.x + x, player_chunk.z + z)
			required_chunks[coord.hash()] = coord
	
	# Unload distant chunks
	var chunks_to_unload = []
	for coord in loaded_chunks:
		if not required_chunks.has(coord.hash()):
			chunks_to_unload.append(coord)
	
	for coord in chunks_to_unload:
		unload_chunk(coord)
	
	# Load required chunks
	for coord in required_chunks.values():
		if not loaded_chunks.has(coord.hash()) and not generating_chunks.has(coord.hash()):
			load_or_generate_chunk(coord)


func _process_generation_queue():
	var count = min(MAX_GENERATIONS_PER_FRAME, generation_queue.size())
	
	for i in range(count):
		if generation_queue.size() == 0:
			break
		
		var coord = generation_queue.pop_front()
		generating_chunks.erase(coord.hash())
		
		try:
			var chunk = chunk_generator.generate_chunk(coord, world)
			loaded_chunks[coord.hash()] = chunk
			emit chunk_loaded(chunk)
		except error:
			push_error("Failed to generate chunk %s: %s" % [coord, error])


func load_or_generate_chunk(coord: ChunkCoordinates):
	generation_queue.append(coord)
	generating_chunks[coord.hash()] = coord


func get_chunk(coord: ChunkCoordinates) -> Chunk:
	if loaded_chunks.has(coord.hash()):
		return loaded_chunks[coord.hash()]
	return null


func has_chunk(coord: ChunkCoordinates) -> bool:
	return loaded_chunks.has(coord.hash())


func get_loaded_chunks() -> Array:
	return loaded_chunks.values()


func unload_chunk(coord: ChunkCoordinates):
	if loaded_chunks.has(coord.hash()):
		var chunk = loaded_chunks[coord.hash()]
		# Save if modified
		if chunk.is_dirty:
			# TODO: Save through WorldStorage
			pass
		
		chunk.cleanup()
		loaded_chunks.erase(coord.hash())
		emit chunk_unloaded(coord)


func clear_all_chunks():
	for chunk in loaded_chunks.values():
		if chunk.is_dirty:
			# TODO: Save through WorldStorage
			pass
		chunk.cleanup()
	loaded_chunks.clear()
	generation_queue.clear()
	generating_chunks.clear()


func cleanup():
	clear_all_chunks()
