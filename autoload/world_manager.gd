extends Node


# Global world manager singleton

# World instance
var world: World = null

# World storage
var world_storage: WorldStorage = null

# Scene for chunk nodes
var chunk_scene: PackedScene = null

# Dictionary of chunk nodes
var chunk_nodes: Dictionary = {}

# Player reference
var player: Node3D = null

# World seed
var world_seed: String = "BarnocraftSeed"


func _ready():
	# Initialize world
	_initialize_world()
	
	# Load chunk scene
	chunk_scene = preload("res://scenes/chunk_node.tscn")
	
	# Connect signals
	get_tree().connect("node_added", _on_node_added)


func _initialize_world():
	world = World.new(world_seed)
	world_storage = WorldStorage.new("world_" + world_seed)


func _on_node_added(node: Node):
	# Check if this is the player
	if node.name == "Player":
		player = node as Node3D
		world.update(player.global_position)


func _process(delta: float):
	if player != null and world != null:
		world.update(player.global_position)
		_update_chunk_nodes()


func _update_chunk_nodes():
	if world == null:
		return
	
	# Get loaded chunks
	var loaded_chunks = world.chunk_manager.get_loaded_chunks()
	
	# Create nodes for new chunks
	for chunk in loaded_chunks:
		var coord_key = chunk.coordinates.hash()
		if not chunk_nodes.has(coord_key):
			_create_chunk_node(chunk)
		elif chunk.needs_mesh_rebuild:
			# Update existing node
			var node = chunk_nodes[coord_key]
			if node != null:
				node._rebuild_mesh()
	
	# Remove nodes for unloaded chunks
	var nodes_to_remove = []
	for coord_key in chunk_nodes:
		var coord = chunk_nodes[coord_key].coordinates
		if not world.chunk_manager.has_chunk(coord):
			nodes_to_remove.append(coord_key)
	
	for coord_key in nodes_to_remove:
		_remove_chunk_node(coord_key)


func _create_chunk_node(chunk: Chunk):
	if chunk_scene == null:
		return
	
	var node = chunk_scene.instantiate()
	if node == null:
		return
	
	add_child(node)
	
	var chunk_node = node as ChunkNode
	chunk_node.initialize(chunk, world)
	
	chunk_nodes[chunk.coordinates.hash()] = chunk_node


func _remove_chunk_node(coord_key: int):
	if not chunk_nodes.has(coord_key):
		return
	
	var node = chunk_nodes[coord_key]
	if node != null:
		node.cleanup()
		node.queue_free()
	
	chunk_nodes.erase(coord_key)


# Public functions
func get_world() -> World:
	return world


func get_block(world_pos: Vector3i) -> BlockType:
	if world != null:
		return world.get_block_world(world_pos)
	return BlockType.AIR


func set_block(world_pos: Vector3i, block_type: BlockType) -> bool:
	if world != null:
		return world.set_block_world(world_pos, block_type)
	return false


func destroy_block(world_pos: Vector3i) -> bool:
	if world != null:
		return world.destroy_block_world(world_pos)
	return false
