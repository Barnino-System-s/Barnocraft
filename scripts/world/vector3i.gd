class_name Vector3i

extends RefCounted


# Integer vector for world coordinates

var x: int = 0
var y: int = 0
var z: int = 0


func _init(p_x: int = 0, p_y: int = 0, p_z: int = 0):
	x = p_x
	y = p_y
	z = p_z


func _to_string() -> String:
	return "(%d, %d, %d)" % [x, y, z]


func to_vector3() -> Vector3:
	return Vector3(x, y, z)


func distance_to(other: Vector3i) -> float:
	var dx = x - other.x
	var dy = y - other.y
	var dz = z - other.z
	return sqrt(dx * dx + dy * dy + dz * dz)


func distance_squared_to(other: Vector3i) -> int:
	var dx = x - other.x
	var dy = y - other.y
	var dz = z - other.z
	return dx * dx + dy * dy + dz * dz


func equals(other: Vector3i) -> bool:
	return x == other.x and y == other.y and z == other.z


func hash() -> int:
	var result = x
	result = (result * 397) ^ y
	result = (result * 397) ^ z
	return result
