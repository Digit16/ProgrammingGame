extends Node2D

@onready var player = get_tree().get_first_node_in_group("player")

var active_areas = []
var can_interact = true

# Assuming player has a direction variable to determine its facing direction
# Directions can be "up", "down", "left", "right"
var player_facing_direction: String

func _ready():
	player.connect("picked_up", Callable(self, "_on_interact"))

func register_area(area: InteractionArea):
	active_areas.push_back(area)

func unregister_area(area: InteractionArea):
	var index = active_areas.find(area)
	if index != -1:
		active_areas.remove_at(index)
		
func _on_interact():
	if can_interact:
		if active_areas.size() > 0:
			can_interact = false
			await active_areas[0].interact.call()
			can_interact = true
