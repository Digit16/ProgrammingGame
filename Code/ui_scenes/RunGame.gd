extends Button

var a = 0

var level_sceenes = [
	"res://levels/level_1.tscn"
]

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_pressed():
	var text = owner.get_node("%MainCodeEditor").text
	var player_node = get_tree().get_first_node_in_group("player")
	for line in text.split("\n"):
		player_node.commands.append(line)
		
	if a == 5:
		change_level()
	a += 1
	

func change_level():
	var levels = get_tree().get_first_node_in_group("levels")
	for level in levels.get_children():
		remove_child(level)
		level.free()
	if level_sceenes.size() > 0:
		var next_level = level_sceenes.pop_front()
		levels.add_child(load(next_level).instantiate())
	
