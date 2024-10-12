extends Node2D

@onready var player = get_tree().get_first_node_in_group("player")
@onready var box = get_tree().get_first_node_in_group("player")

var active_areas = []
var can_interact = true

func _ready():
	player.connect("picked_up", Callable(self, "_on_push"))
	player.connect("can_push", Callable(self, "_on_can_push"))
	
	box.connect("push_result", Callable(self, "_on_push_result"))

func register_area(area: InteractionArea):
	active_areas.push_back(area)

func unregister_area(area: InteractionArea):
	var index = active_areas.find(area)
	if index != -1:
		active_areas.remove_at(index)
		
func _on_push():
	if can_interact:
		if active_areas.size() > 0:
			can_interact = false
			await active_areas[0].interact.call()
			can_interact = true
			
func _on_can_push():
	if can_interact:
		if active_areas.size() > 0:
			can_interact = false
			var can_push = await active_areas[0].can_interact.call()
			can_interact = true
			emit_signal("can_push_result", can_push)  # Emit the result to the player
			return can_push

func _on_push_result(result: bool):
	if result:
		print("The box can be pushed!")
	else:
		print("The box cannot be pushed!")
		
func check_can_push() -> bool:
	if active_areas.size() > 0:
		var can_push = await active_areas[0].can_interact.call()
		return can_push
	return false
