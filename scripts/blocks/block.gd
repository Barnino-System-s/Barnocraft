class_name Block

extends RefCounted


# Block class represents a block type with its properties

signal block_destroyed


var block_type: BlockType
var name: String
var is_solid: bool = true
var is_transparent: bool = false
var is_indestructible: bool = false


func _init(p_block_type: BlockType, p_name: String, p_is_solid: bool = true, p_is_transparent: bool = false, p_is_indestructible: bool = false):
	block_type = p_block_type
	name = p_name
	is_solid = p_is_solid
	is_transparent = p_is_transparent
	is_indestructible = p_is_indestructible


func can_be_destroyed() -> bool:
	return not is_indestructible


func can_be_placed_on(top_block: Block) -> bool:
	# Can only place on solid blocks or air
	return top_block.is_solid or top_block.block_type == BlockType.AIR
