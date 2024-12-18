extends CharacterBody2D

signal done
signal picked_up
signal push
signal can_push

var targetPos = Vector2.ZERO
var facing = Vector2(0, 1) # Default direction facing down
var prevPos = Vector2.ZERO
var waiting = true
var abort = false
var commands = []
var inventory = []
var removal_distance = 32
var can_push_result = false
var result_received = false

@onready var animation_tree = $AnimationTree
@onready var state_machine = animation_tree.get("parameters/playback")
@onready var end = get_tree().get_first_node_in_group("end")
@onready var interaction_manager = get_tree().get_first_node_in_group("interaction_manager")  # Ensure to point to the InteractionManager

func _ready():
	targetPos = self.position
	var root_node = get_tree().root
	print(root_node.name)
	#var file = FileAccess.open("res://commands.txt", FileAccess.READ)
	#if file:
		#while !file.eof_reached():
			#var line = file.get_line()
			#commands.append(line)
			#print(line)
	#file.close()
	
	interaction_manager.connect("can_push_result", Callable(self, "_on_can_push_result"))

func _process(_delta):
	if abort:
		return
	
	if waiting:
		waiting = false
		if commands.size() > 0:
			var c = commands.pop_front()
			match c:
				'moveSouth()':
					_on_node_down()
				'moveNorth()':
					_on_node_up()
				'moveWest()':
					_on_node_left()
				'moveEast()':
					_on_node_right()
				'push()':
					_on_pick_up()
				'rotateLeft()':
					_on_node_rotate_clockwise()
				'rotateRight()':
					_on_node_rotate_c_clockwise()
				'cp':
					_on_check_if_can_push()
				'cm':
					_on_check_if_can_move()
				'platform_count':
					_on_get_platforms_count()
				'bop':
					_on_get_boxes_on_platfor_count()
			animation_tree.set("parameters/Walk/blend_position", facing)
		else:
			pass#state_machine.travel("End") # Idle state if no commands are left
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

func _on_check_if_can_push() -> bool:
	var can_push_result = await interaction_manager.call("check_can_push")
	print("Final push result: ", can_push_result)
	return can_push_result

func _on_check_if_can_move() -> bool:
	var ray_length = 16
	var ray_end = position + (facing.normalized() * ray_length)

	var query = PhysicsRayQueryParameters2D.new()
	query.from = position
	query.to = ray_end
	query.collide_with_areas = false
	query.collide_with_bodies = true

	var space_state = get_world_2d().direct_space_state
	var result = space_state.intersect_ray(query)
	var can_move_result = true
	if result:
		var collider = result["collider"]
		print("Collider detected: ", collider)
		if collider.is_in_group("walls"):
			can_move_result = false

	if can_move_result:
		can_move_result = !await interaction_manager.call("check_can_push")
	print("Final move result: ", can_move_result)
	return can_move_result

func _on_pick_up():
	emit_signal("picked_up")
	
func _on_get_platforms_count():
	var platforms = get_tree().get_nodes_in_group("platforms")
	var platform_count = platforms.size()
	print("Number of platforms: ", platform_count)
	return platform_count

func _on_get_boxes_on_platfor_count():
	var gm = get_tree().get_nodes_in_group("game_manager")[0]
	var occupied_platforms = gm.get_occupied_platforms_count()
	print("Number of boxes on platfroms: ", occupied_platforms)
	return occupied_platforms
	
