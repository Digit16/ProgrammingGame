extends Button

var start_scene = preload("res://ui_scenes/NewStart.tscn")

func _pressed():
	get_tree().change_scene_to_packed(start_scene)
