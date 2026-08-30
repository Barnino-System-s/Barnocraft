class_name ChunkCoordinates

extends RefCounted


# Coordinates for chunk positioning

var x: int = 0
var z: int = 0


func _init(p_x: int = 0, p_z: int = 0):
	x = p_x
	z = p_z


func _to_string() -> String:
	return "Chunk(%d, %d)" % [x, z]


func equals(other: ChunkCoordinates) -> bool:
	return x == other.x and z == other.z


func hash() -> int:
	return (x * 397) ^ z


# Convert world position to chunk coordinates
static func from_world_position(world_x: float, world_z: float) -> ChunkCoordinates:
	var chunk_x = floor(world_x / WorldConstants.CHUNK_SIZE_X)
	var chunk_z = floor(world_z / WorldConstants.CHUNK_SIZE_Z)
	return ChunkCoordinates.new(chunk_x, chunk_z)


static func from_world_position_vector3(world_pos: Vector3) -> ChunkCoordinates:
	return from_world_position(world_pos.x, world_pos.z)


static func from_world_position_int(world_x: int, world_z: int) -> ChunkCoordinates:
	var chunk_x = world_x / WorldConstants.CHUNK_SIZE_X
	if world_x < 0 and world_x % WorldConstants.CHUNK_SIZE_X != 0:
		chunk_x -= 1
	var chunk_z = world_z / WorldConstants.CHUNK_SIZE_Z
	if world_z < 0 and world_z % WorldConstants.CHUNK_SIZE_Z != 0:
		chunk_z -= 1
	return ChunkCoordinates.new(chunk_x, chunk_z)


# Get local position within chunk from world position
static func get_local_position(world_pos: Vector3i) -> Vector3i:
	var local_x = world_pos.x % WorldConstants.CHUNK_SIZE_X
	if local_x < 0:
		local_x += WorldConstants.CHUNK_SIZE_X
	var local_y = world_pos.y % WorldConstants.CHUNK_SIZE_Y
	if local_y < 0:
		local_y += WorldConstants.CHUNK_SIZE_Y
	var local_z = world_pos.z % WorldConstants.CHUNK_SIZE_Z
	if local_z < 0:
		local_z += WorldConstants.CHUNK_SIZE_Z
	return Vector3i.new(local_x, local_y, local_z)
