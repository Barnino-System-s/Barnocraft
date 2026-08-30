class_name ChunkNode

extends MeshInstance3D


# 3D node that represents a chunk in the world

@onready var chunk: Chunk = null
@onready var coordinates: ChunkCoordinates = null

# Mesh builder
var mesh_builder: ChunkMeshBuilder = null

# Reference to world
var world: World = null


func _ready():
	mesh_builder = ChunkMeshBuilder.new()
	if chunk != null:
		_rebuild_mesh()


# Initialize with a chunk
func initialize(p_chunk: Chunk, p_world: World):
	chunk = p_chunk
	world = p_world
	coordinates = chunk.coordinates
	
	# Position the chunk in world space
	position = Vector3(
		coordinates.x * WorldConstants.CHUNK_SIZE_X,
		0,
		coordinates.z * WorldConstants.CHUNK_SIZE_Z
	)
	
	# Connect to chunk signals
	chunk.connect("mesh_needs_rebuild", _on_chunk_needs_rebuild)
	
	_rebuild_mesh()


func _on_chunk_needs_rebuild():
	_rebuild_mesh()


func _rebuild_mesh():
	if chunk == null or mesh_builder == null:
		return
	
	var mesh = mesh_builder.build_mesh(chunk, world)
	if mesh != null:
		self.mesh = mesh
		chunk.mark_mesh_as_rebuilt()


func cleanup():
	if chunk != null:
		chunk.disconnect("mesh_needs_rebuild", self, "_on_chunk_needs_rebuild")
	chunk = null
	world = null
