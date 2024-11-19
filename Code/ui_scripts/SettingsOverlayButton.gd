extends Button

func _pressed():
	get_parent().add_child(load("res://ui_scenes/SettingsOverlay.tscn").instantiate())
