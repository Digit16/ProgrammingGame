extends Node2D

var dragging_offset = Vector2() 
var new_mouse_position = Vector2()

var is_dragging = false
var mouse_on_component = false

@export var control_component_path: NodePath
@onready var control_component = get_node(control_component_path)

func _ready():
	pass
	
	
func _input(event):
	if event is InputEventMouseButton:
		if event.button_index != MOUSE_BUTTON_LEFT:
			return
		
		if event.is_pressed() && mouse_on_component:
			move_in_front()
			
			dragging_offset = get_viewport().get_mouse_position() - position
			new_mouse_position = get_viewport().get_mouse_position() - dragging_offset
			is_dragging = true
		else:
			is_dragging = false
			
	elif event is InputEventMouseMotion:
		if is_dragging:
			new_mouse_position = get_viewport().get_mouse_position() - dragging_offset

func _physics_process(delta):
	if is_dragging:
		position = new_mouse_position.clamp(Vector2.ZERO, Vector2(get_viewport().size) - control_component.size)
		

func mouse_entered():
	mouse_on_component = true

func mouse_exited():
	mouse_on_component = false

func move_in_front():
	get_parent().move_child(self, -1)
