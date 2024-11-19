extends Sprite2D


func _ready():
	#pass
	Input.set_mouse_mode(Input.MOUSE_MODE_HIDDEN)

func _input(event):
	if event is InputEventMouseMotion:
		position = get_viewport().get_mouse_position()

func _exit_tree():
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
