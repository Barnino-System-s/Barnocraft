class_name PlayerController

exports Node3D


# First-person player controller

# Movement settings
@export var walk_speed: float = 5.0
@export var run_speed: float = 8.0
@export var jump_velocity: float = 4.5
@export var gravity: float = -9.81

# Mouse settings
@export var mouse_sensitivity: float = 0.002

# Interaction settings
@export var reach_distance: float = 5.0

# Camera
@onready var camera: Camera3D = $Camera3D

# Character body
@onready var character_body: CharacterBody3D = $CharacterBody3D

# Current velocity
var velocity: Vector3 = Vector3.ZERO

# Is player on ground
var is_on_floor: bool = false

# Current speed
var current_speed: float = 0.0


func _ready():
	# Lock and hide mouse cursor
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED


func _input(event: InputEvent):
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		var mouse_event = event as InputEventMouseMotion
		_rotate_camera(mouse_event.relative)


func _physics_process(delta: float):
	# Get input
	var input_dir = _get_input_direction()
	
	# Check if on floor
	is_on_floor = character_body.is_on_floor()
	
	if is_on_floor:
		velocity.y = -0.1  # Small downward force to keep on floor
	
	# Apply gravity
	if not is_on_floor:
		velocity.y += gravity * delta
	
	# Handle jump
	if is_on_floor and Input.is_action_just_pressed("jump"):
		velocity.y = jump_velocity
	
	# Set speed based on shift key
	current_speed = run_speed if Input.is_action_pressed("run") else walk_speed
	
	# Move in direction
	if input_dir.length() > 0:
		var direction = (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
		velocity.x = direction.x * current_speed
		velocity.z = direction.z * current_speed
	else:
		velocity.x = move_toward(velocity.x, 0, current_speed)
		velocity.z = move_toward(velocity.z, 0, current_speed)
	
	# Move character
	character_body.velocity = velocity
	character_body.move_and_slide()
	
	# Handle block breaking
	if Input.is_action_just_pressed("break_block"):
		_try_break_block()
	
	# Handle block placing
	if Input.is_action_just_pressed("place_block"):
		_try_place_block()


func _get_input_direction() -> Vector2:
	var input = Vector2.ZERO
	input.x = Input.get_action_strength("move_right") - Input.get_action_strength("move_left")
	input.y = Input.get_action_strength("move_back") - Input.get_action_strength("move_forward")
	return input.normalized()


func _rotate_camera(relative: Vector2):
	# Rotate around Y axis (left-right)
	rotate_y(-relative.x * mouse_sensitivity)
	
	# Rotate camera around X axis (up-down)
	camera.rotate_x(-relative.y * mouse_sensitivity)
	
	# Clamp camera rotation
	camera.rotation.x = clamp(camera.rotation.x, deg_to_rad(-90), deg_to_rad(90))


func _try_break_block():
	if camera == null:
		return
	
	var space_state = character_body.get_world_3d().direct_space_state
	var query = PhysicsRayQueryParameters3D.new()
	query.from = camera.global_position
	query.to = camera.global_position + camera.global_transform.basis.z * -reach_distance
	query.collide_with_areas = false
	query.collide_with_bodies = true
	query.collide_with_area_bodies = false
	
	var result = space_state.intersect_ray(query)
	
	if result:
		var world_pos = Vector3i.new(
			floor(result.position.x - result.normal.x * 0.5),
			floor(result.position.y - result.normal.y * 0.5),
			floor(result.position.z - result.normal.z * 0.5)
		)
		
		# Use WorldManager to destroy block
		if WorldManager != null:
			WorldManager.destroy_block(world_pos)


func _try_place_block():
	if camera == null:
		return
	
	var space_state = character_body.get_world_3d().direct_space_state
	var query = PhysicsRayQueryParameters3D.new()
	query.from = camera.global_position
	query.to = camera.global_position + camera.global_transform.basis.z * -reach_distance
	query.collide_with_areas = false
	query.collide_with_bodies = true
	query.collide_with_area_bodies = false
	
	var result = space_state.intersect_ray(query)
	
	if result:
		var world_pos = Vector3i.new(
			floor(result.position.x + result.normal.x * 0.5),
			floor(result.position.y + result.normal.y * 0.5),
			floor(result.position.z + result.normal.z * 0.5)
		)
		
		# Use WorldManager to place block
		if WorldManager != null:
			# TODO: Get block type from inventory
			WorldManager.set_block(world_pos, BlockType.DIRT)
