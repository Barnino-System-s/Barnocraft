class_name Chunk

extends RefCounted


# Chunk class represents a 16x128x16 section of the world

signal chunk_modified
signal mesh_needs_rebuild


# Chunk coordinates in world
var coordinates: ChunkCoordinates

# Reference to world
var world: World

# Block data stored as byte array (block type IDs)
var _blocks: PackedByteArray

# Flags
var is_dirty: bool = false
var needs_mesh_rebuild: bool = true


func _init(p_coordinates: ChunkCoordinates, p_world: World):
	coordinates = p_coordinates
	world = p_world
	_blocks = PackedByteArray()
	_blocks.resize(WorldConstants.CHUNK_VOLUME)
	# Initialize with air
	_blocks.fill(BlockType.AIR)


func _init_with_data(p_coordinates: ChunkCoordinates, p_world: World, p_data: PackedByteArray):
	coordinates = p_coordinates
	world = p_world
	if p_data.size() == WorldConstants.CHUNK_VOLUME:
		_blocks = p_data.duplicate()
	else:
		_blocks = PackedByteArray()
		_blocks.resize(WorldConstants.CHUNK_VOLUME)
		_blocks.fill(BlockType.AIR)


# Get block type at local coordinates
func get_block(local_x: int, local_y: int, local_z: int) -> BlockType:
	if local_x < 0 or local_x >= WorldConstants.CHUNK_SIZE_X:
		return BlockType.AIR
	if local_y < 0 or local_y >= WorldConstants.CHUNK_SIZE_Y:
		return BlockType.AIR
	if local_z < 0 or local_z >= WorldConstants.CHUNK_SIZE_Z:
		return BlockType.AIR
	
	var index = local_x + local_z * WorldConstants.CHUNK_SIZE_X + local_y * WorldConstants.CHUNK_SIZE_X * WorldConstants.CHUNK_SIZE_Z
	return _blocks[index]


func get_block_local(pos: Vector3i) -> BlockType:
	return get_block(pos.x, pos.y, pos.z)


# Set block type at local coordinates
func set_block(local_x: int, local_y: int, local_z: int, block_type: BlockType) -> bool:
	if local_x < 0 or local_x >= WorldConstants.CHUNK_SIZE_X:
		return false
	if local_y < 0 or local_y >= WorldConstants.CHUNK_SIZE_Y:
		return false
	if local_z < 0 or local_z >= WorldConstants.CHUNK_SIZE_Z:
		return false
	
	var index = local_x + local_z * WorldConstants.CHUNK_SIZE_X + local_y * WorldConstants.CHUNK_SIZE_X * WorldConstants.CHUNK_SIZE_Z
	
	var old_block = BlockRegistry.new().get_block(_blocks[index])
	var new_block = BlockRegistry.new().get_block(block_type)
	
	# Check if we can modify this block
	if old_block.is_indestructible and block_type != _blocks[index]:
		return false
	
	if new_block.is_indestructible and _blocks[index] != BlockType.AIR:
		return false
	
	_blocks[index] = block_type
	is_dirty = true
	needs_mesh_rebuild = true
	
	# Mark neighbor chunks for mesh rebuild if on edge
	if local_x == 0 or local_x == WorldConstants.CHUNK_SIZE_X - 1 or \
		local_y == 0 or local_y == WorldConstants.CHUNK_SIZE_Y - 1 or \
		local_z == 0 or local_z == WorldConstants.CHUNK_SIZE_Z - 1:
		_mark_neighbor_chunks_for_rebuild(local_x, local_y, local_z)
	
	emit chunk_modified
	emit mesh_needs_rebuild
	return true


func set_block_local(pos: Vector3i, block_type: BlockType) -> bool:
	return set_block(pos.x, pos.y, pos.z, block_type)


func _mark_neighbor_chunks_for_rebuild(local_x: int, local_y: int, local_z: int):
	if world == null:
		return
	
	if local_x == 0:
		var neighbor_coord = ChunkCoordinates.new(coordinates.x - 1, coordinates.z)
		world.mark_chunk_for_mesh_rebuild(neighbor_coord)
	elif local_x == WorldConstants.CHUNK_SIZE_X - 1:
		var neighbor_coord = ChunkCoordinates.new(coordinates.x + 1, coordinates.z)
		world.mark_chunk_for_mesh_rebuild(neighbor_coord)
	
	if local_z == 0:
		var neighbor_coord = ChunkCoordinates.new(coordinates.x, coordinates.z - 1)
		world.mark_chunk_for_mesh_rebuild(neighbor_coord)
	elif local_z == WorldConstants.CHUNK_SIZE_Z - 1:
		var neighbor_coord = ChunkCoordinates.new(coordinates.x, coordinates.z + 1)
		world.mark_chunk_for_mesh_rebuild(neighbor_coord)


# Get all block data as PackedByteArray
func get_block_data() -> PackedByteArray:
	return _blocks.duplicate()


# Mark chunk as clean (saved)
func mark_as_clean():
	is_dirty = false


# Mark mesh as rebuilt
func mark_mesh_as_rebuilt():
	needs_mesh_rebuild = false
