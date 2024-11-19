extends Node

var x = 0
var y = 0


var start_pos = null
var frame = 0
# Called when the node enters the scene tree for the first time.
func _ready():
	var player_node = get_tree().get_first_node_in_group("player")
	start_pos = player_node.targetPos;


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	frame = frame + 1
	
	if frame % 14 == 0:
		var player_node = get_tree().get_first_node_in_group("player")
		var offset =  player_node.targetPos- start_pos;
		
		var rng = RandomNumberGenerator.new()
		rng.randomize()
		var num = rng.randi_range(0, 4)
		
		if num == 0 and x < 2:
			player_node.commands.append("moveEast()")
			x += 1
		if num == 1 and x > -2:
			player_node.commands.append("moveWest()")
			x -= 1
		if num == 2 and y < 2:
			player_node.commands.append("moveSouth()")
			y += 1
		if num == 3 and y > -2:
			player_node.commands.append("moveNorth()")
			y -= 1
