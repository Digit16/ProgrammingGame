signal up
signal down
signal left
signal right

var next_command = true

func _ready():
	var file = FileAccess.open("res://commands.txt", FileAccess.READ)
	if file:
		var line = file.get_line()
		while line != "":
			process_command(line)
			await execute_next_command()
			line = file.get_line()
	file.close()

func process_command(command):
	# Parse the command and execute corresponding actions
	command = command.strip_edges()  # Remove leading and trailing whitespaces
	match command:
		"u":
			emit_signal("up")
		"d":
			emit_signal("down")
		"l":
			emit_signal("left")
		"r":
			emit_signal("right")
		_:
			print("Invalid command:", command)
