extends Node

@onready var platforms = get_tree().get_nodes_in_group("platforms")
@onready var boxes = get_tree().get_nodes_in_group("boxes")

var all_platforms = []

func _ready():
	# Initialize platforms and boxes
	_initialize_platforms()
	_initialize_boxes()

func _initialize_platforms():
	all_platforms.clear()
	for platform in platforms:
		all_platforms.append({
			"platform": platform,
			"occupied": false
		})
		
func get_occupied_platforms_count():
	var occupied_count = 0
	for platform_dict in all_platforms:
		if platform_dict["occupied"] == true:
			occupied_count += 1
	return occupied_count

func _initialize_boxes():
	# Ensure boxes are initialized after they are added to the scene
	for box in boxes:
		box.connect("on_platform_sig", Callable(self, "_on_box_moved_in"))
		box.connect("out_platform_sig", Callable(self, "_on_box_moved_out"))

func _on_box_moved_in(platform):
	_check_all_platforms_occupied(platform)

func _on_box_moved_out(platform):
	for platform_info in all_platforms:
			if platform_info["platform"] == platform:
				platform_info["occupied"] = false
				print("Updated platform state:", platform)
				break

func _check_all_platforms_occupied(platform):
	for platform_info in all_platforms:
		if platform_info["platform"] == platform:
			platform_info["occupied"] = true
			print("Updated platform state:", platform_info)
			break
	
	var all_occupied = true
	for platform_info in all_platforms:
		if not platform_info["occupied"]:
			all_occupied = false
			break

func _load_next_level():
	print("All platforms are occupied! Loading next level...")
	var next_level_path = "res://levels/level_1.tscn"
	if ResourceLoader.exists(next_level_path):
		get_tree().change_scene_to_file(next_level_path)
