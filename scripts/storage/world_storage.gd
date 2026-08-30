class_name WorldStorage

extends RefCounted


# Interface for saving and loading chunks

# Base directory for world saves
var world_path: String = ""


func _init(p_world_name: String):
	world_path = "user://saves/%s/" % p_world_name
	# Create directory if it doesn't exist
	var dir = Directory.new()
	if not dir.dir_exists(world_path):
		dir.make_dir_recursive(world_path)


# Save a chunk to disk
func save_chunk(chunk: Chunk) -> bool:
	try:
		var chunk_path = _get_chunk_path(chunk.coordinates)
		var data = chunk.get_block_data()
		
		var file = FileAccess.open(chunk_path, FileAccess.WRITE)
		if file == null:
			push_error("Failed to open file for writing: %s" % chunk_path)
			return false
		
		file.store_buffer(data)
		file.close()
		return true
		except error:
		push_error("Failed to save chunk %s: %s" % [chunk.coordinates, error])
		return false


# Load a chunk from disk
func load_chunk(coord: ChunkCoordinates, world: World) -> Chunk:
	var chunk_path = _get_chunk_path(coord)
	
	if not FileAccess.file_exists(chunk_path):
		return null
	
	try:
		var file = FileAccess.open(chunk_path, FileAccess.READ)
		if file == null:
			push_error("Failed to open file for reading: %s" % chunk_path)
			return null
		
		var data = PackedByteArray()
		data = file.get_buffer(file.get_length())
		file.close()
		
		return Chunk.new(coord, world, data)
		except error:
		push_error("Failed to load chunk %s: %s" % [coord, error])
		return null


# Check if chunk exists on disk
func chunk_exists(coord: ChunkCoordinates) -> bool:
	return FileAccess.file_exists(_get_chunk_path(coord))


# Delete a chunk from disk
func delete_chunk(coord: ChunkCoordinates) -> bool:
	var chunk_path = _get_chunk_path(coord)
	try:
		var file = FileAccess.open(chunk_path, FileAccess.WRITE)
		if file != null:
			file.close()
			return false
		OS.remove(chunk_path)
		return true
		except error:
		push_error("Failed to delete chunk %s: %s" % [coord, error])
		return false


func _get_chunk_path(coord: ChunkCoordinates) -> String:
	# Format: user://saves/worldname/chunks/x/z.dat
	var chunk_dir = "%s/chunks/%d/%d/" % [world_path, coord.x, coord.z]
	
	var dir = Directory.new()
	if not dir.dir_exists(chunk_dir):
		dir.make_dir_recursive(chunk_dir)
	
	return "%s.dat" % chunk_dir


func cleanup():
	pass
