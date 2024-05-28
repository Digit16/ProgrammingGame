extends CharacterBody2D

var targetPos = Vector2.ZERO
var walk_l : StringName
var walk_r : StringName
var walk_d : StringName
var walk_u : StringName
var facing = Vector2.ZERO

func _ready():
	targetPos = self.position
	walk_l = "walk_left"
	walk_r = "walk_right"
	walk_d = "walk_down"
	walk_u = "walk_up"

# Funkcja _process będzie wykonywana co klatkę, aby aktualizować pozycję postaci
func _process(_delta):
	if self.position.distance_to(targetPos) > 1.0: 
		if facing.x < 0:
			$Character/AnimationPlayer.play(walk_l)
		elif facing.x > 0:
			$Character/AnimationPlayer.play(walk_r)
		elif facing.y < 0:
			$Character/AnimationPlayer.play(walk_u)
		elif facing.y > 0:
			$Character/AnimationPlayer.play(walk_d)
		
		# Update the position using linear interpolation
		self.position = self.position.lerp(targetPos, 0.1)
	else:
		if $Character/AnimationPlayer.current_animation != "":
			if $Character/AnimationPlayer.is_playing():
				$Character/AnimationPlayer.stop()

# Ustawienie docelowej pozycji po naciśnięciu przycisku
func _on_right_pressed():
	facing = Vector2.ZERO
	facing.x = 1
	targetPos.x += 16

func _on_left_pressed():
	facing = Vector2.ZERO
	facing.x = -1
	targetPos.x -= 16

func _on_up_pressed():
	facing = Vector2.ZERO
	facing.y = -1
	targetPos.y -= 16

func _on_down_pressed():
	facing = Vector2.ZERO
	facing.y = 1
	targetPos.y += 16
