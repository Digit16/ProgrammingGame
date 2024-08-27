extends CharacterBody2D

@onready var interaction_area: InteractionArea = $InteractionArea
@onready var sprite = $Sprite2D
@onready var player = get_tree().get_first_node_in_group("player")

func _ready():
	interaction_area.interact = Callable(self, "_on_interact")
	
func _on_interact():
	player.inventory.push_back("box")
	queue_free()
