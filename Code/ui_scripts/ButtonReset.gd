extends Button


# Called when the node enters the scene tree for the first time.
func _ready():
	var button_run_node = get_parent().get_node("ButtonRun")
	button_run_node.reset_level()



