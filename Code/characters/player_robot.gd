extends CharacterBody2D

signal done

var targetPos = Vector2.ZERO
var facing = Vector2.ZERO
var prevPos = Vector2.ZERO
var waiting = true
var abort = false
var commands = []

@onready var animation_tree = $AnimationTree
@onready var state_machine = animation_tree.get("parameters/playback")

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
		waiting = false;
		var c = commands.pop_front()
		if c == 'd':
			_on_node_down()
		if c == 'u':
			_on_node_up()
		if c == 'l':
			_on_node_left()
		if c == 'r':
			_on_node_right()
		animation_tree.set("parameters/Walk/blend_position", facing)
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
					state_machine.travel("hit_down")
				waiting = true

func _on_node_down():
	facing = Vector2.ZERO
	facing.y = 1
	prevPos = targetPos
	targetPos.y += 16


func _on_node_left():
	facing = Vector2.ZERO
	facing.x = -1
	prevPos = targetPos
	targetPos.x -= 16


func _on_node_right():
	#$Character/AnimationPlayer.play("walk_right")
	facing = Vector2.ZERO
	facing.x = 1
	prevPos = targetPos
	targetPos.x += 16


func _on_node_up():
	#$Character/AnimationPlayer.play("walk_up")
	facing = Vector2.ZERO
	facing.y = -1
	prevPos = targetPos
	targetPos.y -= 16
