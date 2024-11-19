extends CodeEdit


var supported_chars = "abcdefghijklmnopqrstuvwxyzABC 1234567890_(){}[];.-+=<>!&|/\\\n"
var supported_unicodes = []

var supported_chars_regex = r"[\w\s\n\[\]{}()<>.,;:?!@#$%^&*-+=|\\\/]"

var _current_executing_line = 0

func _ready() -> void:
	for i in supported_chars.length():
		supported_unicodes.push_back(supported_chars.unicode_at(i))
		

func change_executing_line(line: int) -> void:
	clear_executing_lines()
	set_line_as_executing(line, true)
	for i in range(get_line_count()):		
		set_line_background_color(i, get_theme_color("background_color"))
	set_line_background_color(line, Color(0.2, 0.2, 0.2))
	_current_executing_line = line
	
func advance_executing_line() -> void:
	if _current_executing_line < get_line_count() - 1:
		change_executing_line(_current_executing_line + 1)

func is_char_valid(c):
	var regex = RegEx.new()
	regex.compile(r"[\w\s\n\[\]{}()<>.,;:?!@#$%^&*-+=|\\\/]")
	return regex.search(c) != null

func count_supported_chars(val: String) -> int:
	var count = 0
	for c in val:
		if is_char_valid(c):
			count += 1
	return count

func strip_unsuporrted_chars():
	var carret_col = get_caret_column()
	var carret_row = get_caret_line()

	
	var filtered_text = ""

	for char in text:
		if is_char_valid(char):
			filtered_text += char
	
	
	var current_line = get_line(carret_row)
	var valid_chars_before_cursor = count_supported_chars(current_line.left(carret_col))
	
	#text = ""
	print(filtered_text)
	clear()
	insert_text_at_caret(filtered_text)
	
	set_caret_line(carret_row)
	set_caret_column(valid_chars_before_cursor)


func play_typing_sound():
	var audio = get_node("typing")
	if not audio.is_playing() or true:
		audio.play()

func _on_text_changed():
	strip_unsuporrted_chars()
	print("Hello!")
	

func _on_caret_changed():
	#change_executing_line(get_caret_line())
	pass



func _on_gui_input(event):
	if event is InputEventKey:
		if event.pressed == true and event.echo == false:
			play_typing_sound()


func _on_button_step_pressed():
	advance_executing_line()


func mouse_entered():
	pass # Replace with function body.
