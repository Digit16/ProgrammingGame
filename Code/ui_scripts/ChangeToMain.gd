extends Node

func _on_pressed():
	get_tree().change_scene_to_packed(load("res://ui_scenes/Main.tscn"))
