extends Node

func _pressed():
	self.add_child(load("res://ui_scenes/Credits.tscn").instantiate())
