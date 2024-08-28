extends CharacterBody2D

@onready var interaction_area: InteractionArea = $InteractionArea
@onready var sprite = $Sprite2D
@onready var player = get_tree().get_first_node_in_group("player")
@onready var area2d = $Area2D  # Reference to the Area2D node

var tile_size = Vector2(16, 16) 
var on_platform = false
var previous_position = Vector2.ZERO  # Initialize with a default value
signal position_changed
signal on_platform_sig

func _ready():
	interaction_area.interact = Callable(self, "_on_interact")
	previous_position = position
	area2d.connect("area_entered", Callable(self, "_on_area_entered"))
	area2d.connect("area_exited", Callable(self, "_on_area_exited"))

func _process(delta):
	# Detect if position has changed
	if position != previous_position:
		emit_signal("position_changed")
		previous_position = position

func _on_interact():
	var direction = (position - floor(player.position)).normalized()
	var new_position = position + direction * tile_size
	position = new_position
	emit_signal("position_changed")

func _on_area_entered(area):
	if area.is_in_group("platforms"):
		on_platform = true
		print("Box is on the platform!")
		emit_signal("on_platform_sig", area)

func _on_area_exited(area):
	if area.is_in_group("platforms"):
		on_platform = false
		print("Box left the platform!")
