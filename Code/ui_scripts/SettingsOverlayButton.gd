extends Button

func _pressed():
	owner.add_child(load("res://scenes/SettingsOverlay.tscn").instantiate())
