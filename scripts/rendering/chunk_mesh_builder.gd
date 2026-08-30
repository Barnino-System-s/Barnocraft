class_name ChunkMeshBuilder

exports RefCounted


# Builds mesh data for a chunk

# Block UV mapping
const BLOCK_UVS: Dictionary = {
	BlockType.GRASS_BLOCK: Vector2(0, 0),
	BlockType.DIRT: Vector2(0, 1),
	BlockType.COBBLESTONE: Vector2(1, 0),
	BlockType.COBBLESTONE_2: Vector2(1, 1),
	BlockType.SUPERBLOCK: Vector2(2, 0),
	BlockType.AIR: Vector2(0, 0)
}

const UV_SIZE: float = 0.1


# Build mesh for a chunk
func build_mesh(chunk: Chunk, world: World) -> ArrayMesh:
	var surface_tool = SurfaceTool.new()
	surface_tool.begin(Mesh.PRIMITIVE_TRIANGLES)
	
	for local_x in range(WorldConstants.CHUNK_SIZE_X):
		for local_y in range(WorldConstants.CHUNK_SIZE_Y):
			for local_z in range(WorldConstants.CHUNK_SIZE_Z):
				var block_type = chunk.get_block(local_x, local_y, local_z)
				
				if block_type == BlockType.AIR:
					continue
				
				# Check if each face should be rendered
				var render_front = _should_render_face(chunk, world, local_x, local_y, local_z, 0, 0, 1)
				var render_back = _should_render_face(chunk, world, local_x, local_y, local_z, 0, 0, -1)
				var render_left = _should_render_face(chunk, world, local_x, local_y, local_z, -1, 0, 0)
				var render_right = _should_render_face(chunk, world, local_x, local_y, local_z, 1, 0, 0)
				var render_top = _should_render_face(chunk, world, local_x, local_y, local_z, 0, 1, 0)
				var render_bottom = _should_render_face(chunk, world, local_x, local_y, local_z, 0, -1, 0)
				
				var pos = Vector3(local_x, local_y, local_z)
				var uv = BLOCK_UVS.get(block_type, Vector2(0, 0))
				
				if render_front:
					_add_front_face(surface_tool, pos, uv)
				if render_back:
					_add_back_face(surface_tool, pos, uv)
				if render_left:
					_add_left_face(surface_tool, pos, uv)
				if render_right:
					_add_right_face(surface_tool, pos, uv)
				if render_top:
					_add_top_face(surface_tool, pos, uv)
				if render_bottom:
					_add_bottom_face(surface_tool, pos, uv)
	
	# Generate normals, tangents, etc.
	surface_tool.generate_normals()
	surface_tool.generate_tangents()
	
	# Commit to mesh
	var mesh = surface_tool.commit()
	return mesh


func _should_render_face(chunk: Chunk, world: World, x: int, y: int, z: int, offset_x: int, offset_y: int, offset_z: int) -> bool:
	var neighbor_x = x + offset_x
	var neighbor_y = y + offset_y
	var neighbor_z = z + offset_z
	
	var neighbor_block_type: BlockType
	
	# Check if neighbor is within this chunk
	if neighbor_x >= 0 and neighbor_x < WorldConstants.CHUNK_SIZE_X and \
		neighbor_y >= 0 and neighbor_y < WorldConstants.CHUNK_SIZE_Y and \
		neighbor_z >= 0 and neighbor_z < WorldConstants.CHUNK_SIZE_Z:
		neighbor_block_type = chunk.get_block(neighbor_x, neighbor_y, neighbor_z)
	else:
		# Neighbor is in another chunk
		var chunk_x_offset = 0
		if neighbor_x < 0:
			chunk_x_offset = -1
		elif neighbor_x >= WorldConstants.CHUNK_SIZE_X:
			chunk_x_offset = 1
		
		var chunk_z_offset = 0
		if neighbor_z < 0:
			chunk_z_offset = -1
		elif neighbor_z >= WorldConstants.CHUNK_SIZE_Z:
			chunk_z_offset = 1
		
		var neighbor_chunk_coord = ChunkCoordinates.new(
			chunk.coordinates.x + chunk_x_offset,
			chunk.coordinates.z + chunk_z_offset
		)
		
		var neighbor_chunk = world.chunk_manager.get_chunk(neighbor_chunk_coord)
		if neighbor_chunk == null:
			# If no neighbor chunk, render the face
			return true
		
		# Adjust coordinates for neighbor chunk
		var local_neighbor_x = neighbor_x
		if neighbor_x < 0:
			local_neighbor_x = WorldConstants.CHUNK_SIZE_X - 1
		elif neighbor_x >= WorldConstants.CHUNK_SIZE_X:
			local_neighbor_x = 0
		
		var local_neighbor_z = neighbor_z
		if neighbor_z < 0:
			local_neighbor_z = WorldConstants.CHUNK_SIZE_Z - 1
		elif neighbor_z >= WorldConstants.CHUNK_SIZE_Z:
			local_neighbor_z = 0
		
		neighbor_block_type = neighbor_chunk.get_block(local_neighbor_x, neighbor_y, local_neighbor_z)
	
	var registry = BlockRegistry.new()
	var neighbor_block = registry.get_block(neighbor_block_type)
	return not neighbor_block.is_solid or neighbor_block.is_transparent


func _add_front_face(surface_tool: SurfaceTool, position: Vector3, uv: Vector2):
	var v0 = position + Vector3(0, 0, 1)
	var v1 = position + Vector3(1, 0, 1)
	var v2 = position + Vector3(1, 1, 1)
	var v3 = position + Vector3(0, 1, 1)
	
	_add_face(surface_tool, [v0, v1, v2, v3], uv)


func _add_back_face(surface_tool: SurfaceTool, position: Vector3, uv: Vector2):
	var v0 = position + Vector3(1, 0, 0)
	var v1 = position + Vector3(0, 0, 0)
	var v2 = position + Vector3(0, 1, 0)
	var v3 = position + Vector3(1, 1, 0)
	
	_add_face(surface_tool, [v0, v1, v2, v3], uv)


func _add_left_face(surface_tool: SurfaceTool, position: Vector3, uv: Vector2):
	var v0 = position + Vector3(0, 0, 0)
	var v1 = position + Vector3(0, 0, 1)
	var v2 = position + Vector3(0, 1, 1)
	var v3 = position + Vector3(0, 1, 0)
	
	_add_face(surface_tool, [v0, v1, v2, v3], uv)


func _add_right_face(surface_tool: SurfaceTool, position: Vector3, uv: Vector2):
	var v0 = position + Vector3(1, 0, 1)
	var v1 = position + Vector3(1, 0, 0)
	var v2 = position + Vector3(1, 1, 0)
	var v3 = position + Vector3(1, 1, 1)
	
	_add_face(surface_tool, [v0, v1, v2, v3], uv)


func _add_top_face(surface_tool: SurfaceTool, position: Vector3, uv: Vector2):
	var v0 = position + Vector3(0, 1, 1)
	var v1 = position + Vector3(1, 1, 1)
	var v2 = position + Vector3(1, 1, 0)
	var v3 = position + Vector3(0, 1, 0)
	
	_add_face(surface_tool, [v0, v1, v2, v3], uv)


func _add_bottom_face(surface_tool: SurfaceTool, position: Vector3, uv: Vector2):
	var v0 = position + Vector3(0, 0, 0)
	var v1 = position + Vector3(1, 0, 0)
	var v2 = position + Vector3(1, 0, 1)
	var v3 = position + Vector3(0, 0, 1)
	
	_add_face(surface_tool, [v0, v1, v2, v3], uv)


func _add_face(surface_tool: SurfaceTool, vertices: Array, uv: Vector2):
	var vertex_count = surface_tool.get_vertex_count()
	
	for v in vertices:
		surface_tool.add_vertex(v)
	
	# Add triangles (two triangles for quad)
	surface_tool.add_index(vertex_count + 0)
	surface_tool.add_index(vertex_count + 1)
	surface_tool.add_index(vertex_count + 2)
	surface_tool.add_index(vertex_count + 0)
	surface_tool.add_index(vertex_count + 2)
	surface_tool.add_index(vertex_count + 3)
	
	# Add UV coordinates
	var uv_size = UV_SIZE
	surface_tool.add_uv(Vector2(uv.x, uv.y))
	surface_tool.add_uv(Vector2(uv.x + uv_size, uv.y))
	surface_tool.add_uv(Vector2(uv.x + uv_size, uv.y + uv_size))
	surface_tool.add_uv(Vector2(uv.x, uv.y + uv_size))
