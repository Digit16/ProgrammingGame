extends TextureRect


var dragging_offset

var is_dragging = false
var mouse_on_component = false

var resize_offset: Vector2
var distance_moved: Vector2

@export var resized_component_path: NodePath
@onready var resized_component = get_node(resized_component_path)


func _input(event):
	if event is InputEventMouseButton:
		if event.button_index != MOUSE_BUTTON_LEFT:
			return
			
		if event.is_pressed() && mouse_on_component:
			resize_offset = get_viewport().get_mouse_position() - resized_component.size
			distance_moved =  get_viewport().get_mouse_position() - resize_offset
			is_dragging = true
		else:
			is_dragging = false
			
	elif event is InputEventMouseMotion:
		if is_dragging:
			distance_moved =  get_viewport().get_mouse_position() - resize_offset

func _physics_process(delta):
	if is_dragging:
		resized_component.size = distance_moved
		


func mouse_entered():
	mouse_on_component = true
	
func mouse_exited():
	mouse_on_component = false
