class_name BlockRegistry

extends RefCounted


# Static registry for all block types

var blocks: Dictionary = {}


func _init():
	_init_blocks()


func _init_blocks():
	# Register all block types
	blocks[BlockType.AIR] = Block.new(BlockType.AIR, "Air", false, true, false)
	blocks[BlockType.GRASS_BLOCK] = Block.new(BlockType.GRASS_BLOCK, "Grass Block", true, false, false)
	blocks[BlockType.DIRT] = Block.new(BlockType.DIRT, "Dirt", true, false, false)
	blocks[BlockType.COBBLESTONE] = Block.new(BlockType.COBBLESTONE, "Cobblestone", true, false, false)
	blocks[BlockType.COBBLESTONE_2] = Block.new(BlockType.COBBLESTONE_2, "Cobblestone 2", true, false, false)
	blocks[BlockType.SUPERBLOCK] = Block.new(BlockType.SUPERBLOCK, "Superblock", true, false, true)


func get_block(block_type: BlockType) -> Block:
	if blocks.has(block_type):
		return blocks[block_type]
	push_error("Block type not found: %s" % block_type)
	return blocks[BlockType.AIR]


func get_block_by_id(block_id: int) -> Block:
	var type = block_id
	if type >= 0 and type < blocks.size():
		return blocks[type]
	return blocks[BlockType.AIR]
