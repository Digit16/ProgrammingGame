extends Button

@export var scene_to_spawn: PackedScene

func _pressed():
	owner.add_child(scene_to_spawn.instantiate())
