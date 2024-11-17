extends Button

var frame = 0

var level_sceenes = [
	"res://levels/level_0.tscn",
	"res://levels/level_1.tscn",
	"res://levels/level_3.tscn",
	"res://levels/level_4.tscn",
	"res://levels/level_5.tscn"
]

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	frame += 1
	if frame % 30 == 0:
		var player_node = get_tree().get_first_node_in_group("player")
		var platform_count = player_node._on_get_platforms_count()
		var valid_box_count = player_node._on_get_boxes_on_platfor_count()
		
		if platform_count == valid_box_count:
			var level_popup = get_tree().get_first_node_in_group("level_popup")
			level_popup.visible = true
			


func _on_pressed():
	var text = owner.get_node("%MainCodeEditor").text
	var popup_node = get_tree().get_first_node_in_group("error_popup")
	var popup_text_node = get_tree().get_first_node_in_group("error_popup_text")
	
	
	var interpreter = CodeInterpreter.new()
	var res = interpreter.interpret(text)
	
	if len(res) > 0:
		popup_text_node.text = res
		popup_node.visible = true
		return
	
	popup_node.visible = false
	
	var seq = interpreter.getSequence()

	var player_node = get_tree().get_first_node_in_group("player")
	player_node.commands += seq

	#change_level()
	

func change_level():
	var levels = get_tree().get_first_node_in_group("levels")
	for level in levels.get_children():
		remove_child(level)
		level.free()
	if level_sceenes.size() > 1:
		level_sceenes.pop_front()
		levels.add_child(load(level_sceenes[0]).instantiate())
	
	
func reset_level():
	var levels = get_tree().get_first_node_in_group("levels")
	for level in levels.get_children():
		remove_child(level)
		level.free()
	if level_sceenes.size() > 0:
		levels.add_child(load(level_sceenes[0]).instantiate())


func _on_button_reset_pressed():
	reset_level()


func _on_level_ok_button_pressed():
	change_level()
