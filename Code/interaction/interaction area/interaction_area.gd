extends Area2D
class_name InteractionArea

@export var action_name: String = "interact"

@onready var interaction_manager = get_tree().get_first_node_in_group("interaction_manager")  # Ensure to point to the InteractionManager


var interact: Callable = func():
	pass

var can_interact: Callable = func():
	pass

func _on_body_entered(body):
	if body.is_in_group("player"):
		interaction_manager.register_area(self)


func _on_body_exited(body):
	if body.is_in_group("player"):
		interaction_manager.unregister_area(self)
