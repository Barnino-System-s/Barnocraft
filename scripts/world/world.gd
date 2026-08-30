class_name World

extends RefCounted


# Main world class that manages chunks and provides block access

signal block_changed(world_position: Vector3i, old_type: BlockType, new_type: BlockType)


# World seed for deterministic generation
var seed: String = ""

# Chunk manager
var chunk_manager: ChunkManager

# Chunk generator
var chunk_generator: ChunkGenerator

# Block registry
var block_registry: BlockRegistry


func _init(p_seed: String = "default"):
	seed = p_seed
	block_registry = BlockRegistry.new()
	chunk_generator = ChunkGenerator.new(seed)
	chunk_manager = ChunkManager.new(self, chunk_generator)


# Get block at world coordinates
func get_block(world_x: int, world_y: int, world_z: int) -> BlockType:
	var chunk_coord = ChunkCoordinates.from_world_position_int(world_x, world_z)
	var local_pos = ChunkCoordinates.get_local_position(Vector3i.new(world_x, world_y, world_z))
	
	var chunk = chunk_manager.get_chunk(chunk_coord)
	if chunk == null:
		return BlockType.AIR
	
	return chunk.get_block(local_pos.x, local_pos.y, local_pos.z)


func get_block_world(pos: Vector3i) -> BlockType:
	return get_block(pos.x, pos.y, pos.z)


# Set block at world coordinates
func set_block(world_x: int, world_y: int, world_z: int, block_type: BlockType) -> bool:
	var chunk_coord = ChunkCoordinates.from_world_position_int(world_x, world_z)
	var local_pos = ChunkCoordinates.get_local_position(Vector3i.new(world_x, world_y, world_z))
	
	var chunk = chunk_manager.get_chunk(chunk_coord)
	if chunk == null:
		return false
	
	var old_type = chunk.get_block(local_pos.x, local_pos.y, local_pos.z)
	
	# Check SUPERBLOCK
	var old_block = block_registry.get_block(old_type)
	if old_block.is_indestructible and block_type != old_type:
		return false
	
	var new_block = block_registry.get_block(block_type)
	if new_block.is_indestructible and old_type != BlockType.AIR:
		return false
	
	var success = chunk.set_block(local_pos.x, local_pos.y, local_pos.z, block_type)
	if success:
		emit block_changed(Vector3i.new(world_x, world_y, world_z), old_type, block_type)
	return success


func set_block_world(pos: Vector3i, block_type: BlockType) -> bool:
	return set_block(pos.x, pos.y, pos.z, block_type)


# Destroy block (set to air)
func destroy_block(world_x: int, world_y: int, world_z: int) -> bool:
	return set_block(world_x, world_y, world_z, BlockType.AIR)


func destroy_block_world(pos: Vector3i) -> bool:
	return destroy_block(pos.x, pos.y, pos.z)


# Mark a chunk for mesh rebuild
func mark_chunk_for_mesh_rebuild(chunk_coord: ChunkCoordinates):
	var chunk = chunk_manager.get_chunk(chunk_coord)
	if chunk != null:
		chunk.needs_mesh_rebuild = true


# Update world (load/unload chunks)
func update(player_position: Vector3):
	chunk_manager.update_player_position(player_position)
	chunk_manager.update()


func cleanup():
	if chunk_manager != null:
		chunk_manager.cleanup()
	if chunk_generator != null:
		chunk_generator.cleanup()
