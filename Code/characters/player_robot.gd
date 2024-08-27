extends CharacterBody2D

signal done
signal picked_up

var targetPos = Vector2.ZERO
var facing = Vector2(0, 1) # Default direction facing down
var prevPos = Vector2.ZERO
var waiting = true
var abort = false
var commands = []
var inventory = []

@onready var animation_tree = $AnimationTree
@onready var state_machine = animation_tree.get("parameters/playback")
@onready var end = get_tree().get_first_node_in_group("end")

func _ready():
	targetPos = self.position
	var file = FileAccess.open("res://commands.txt", FileAccess.READ)
	if file:
		while !file.eof_reached():
			var line = file.get_line()
			commands.append(line)
			print(line)
	file.close()

func _process(_delta):
	if abort:
		return
	
	if waiting:
		waiting = false
		if commands.size() > 0:
			var c = commands.pop_front()
			match c:
				'd':
					_on_node_down()
				'u':
					_on_node_up()
				'l':
					_on_node_left()
				'r':
					_on_node_right()
				'p':
					_on_pick_up()
				'rc':
					_on_node_rotate_clockwise()
				'rcc':
					_on_node_rotate_c_clockwise()
			animation_tree.set("parameters/Walk/blend_position", facing)
		else:
			state_machine.travel("End") # Idle state if no commands are left
	else:
		self.position = self.position.lerp(targetPos, 4 * _delta)
		var collision = move_and_slide()
		if collision:
			print("collision")
			position = prevPos
			targetPos = prevPos
			state_machine.travel("hit_down")
			abort = true
		else:
			if self.position.distance_to(targetPos) < 1.0: 
				if commands.is_empty():
					emit_signal("done")
				waiting = true
				if _check_plate_reached():
					abort = true

func _check_plate_reached():
	if self.position.distance_to(end.position) < 1.0:
		print("Plate reached! Loading next level.")
		animation_tree.active = false
		return true

func _on_node_down():
	facing = Vector2(0, 1)
	prevPos = targetPos
	targetPos.y += 16

func _on_node_left():
	facing = Vector2(-1, 0)
	prevPos = targetPos
	targetPos.x -= 16

func _on_node_right():
	facing = Vector2(1, 0)
	prevPos = targetPos
	targetPos.x += 16

func _on_node_up():
	facing = Vector2(0, -1)
	prevPos = targetPos
	targetPos.y -= 16

func _on_node_rotate_clockwise():
	facing = Vector2(facing.y, -facing.x)
	print("Rotated clockwise. New facing:", facing)
	animation_tree.set("parameters/Walk/blend_position", facing)

func _on_node_rotate_c_clockwise():
	facing = Vector2(-facing.y, facing.x)
	print("Rotated counter-clockwise. New facing:", facing)
	animation_tree.set("parameters/Walk/blend_position", facing)

var removal_distance = 32

func _on_pick_up():
	emit_signal("picked_up")
