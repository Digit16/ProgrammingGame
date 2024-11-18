extends CharacterBody2D

@onready var interaction_area: InteractionArea = $InteractionArea
@onready var sprite = $Sprite2D
@onready var player = get_tree().get_first_node_in_group("player")
@onready var area2d = $Area2D  # Reference to the Area2D node

var tile_size = Vector2(16, 16) 
var on_platform = null
var previous_position = Vector2.ZERO  # Initialize with a default value
signal position_changed
signal on_platform_sig
signal out_platform_sig
signal push_result(result: bool) 

func _ready():
	add_to_group("box")
	interaction_area.interact = Callable(self, "_on_interact")
	interaction_area.can_interact = Callable(self, "_on_can_interact")
	previous_position = position
	area2d.connect("area_entered", Callable(self, "_on_area_entered"))
	area2d.connect("area_exited", Callable(self, "_on_area_exited"))

func _process(delta):
	# Detect if position has changed
	if position != previous_position:
		emit_signal("position_changed")
		previous_position = position

func _on_interact():
	if _on_can_interact():
		var direction = (position - player.position).normalized()
		direction.x = round(direction.x)
		direction.y = round(direction.y)
		var new_position = position + direction * tile_size
		position = new_position
		emit_signal("position_changed")
	
func _on_can_interact() -> bool:
	var direction_to_box = (position - player.position).normalized()
	var player_facing_direction = player.facing.normalized()
	var is_next_to_box = abs(player.position.x - position.x) <= (tile_size.x * 1.5) and abs(player.position.y - position.y) <= (tile_size.y * 1.5)
	if !is_next_to_box:
		print(abs(player.position.x - position.x))
		print(abs(player.position.y - position.y))
	
	var is_facing_box = direction_to_box.dot(player_facing_direction) > 0.9
	var can_interact = is_next_to_box and is_facing_box
	emit_signal("push_result", can_interact)  # Emit result when checking if the box can be pushed
	return can_interact

func _on_area_entered(area):
	if area.is_in_group("platforms"):
		on_platform = area
		print("Box is on the platform!")
		emit_signal("on_platform_sig", area)
		
func _on_area_exited(area):
	if area.is_in_group("platforms"):
		print("Box has left the platform!")
		emit_signal("out_platform_sig", on_platform)
		on_platform = null
	

