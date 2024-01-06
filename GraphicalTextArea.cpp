#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GraphicalTextArea.h"



bool GraphicalTextArea::keyInput(int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) return false;

	bool shift_mod = mods & GLFW_MOD_SHIFT;
	bool controlMod = mods & GLFW_MOD_CONTROL;

	switch (key) {
	case GLFW_KEY_ENTER:
		type('\n');
		return true;
	case GLFW_KEY_BACKSPACE:
		backspace();
		return true;
	case GLFW_KEY_DELETE:
		del();
		return true;
	case GLFW_KEY_UP:
		moveCursorUp(shift_mod);
		return true;
	case GLFW_KEY_DOWN:
		moveCursorDown(shift_mod);
		return true;
	case GLFW_KEY_LEFT:
		moveCursorLeft(shift_mod);
		return true;
	case GLFW_KEY_RIGHT:
		moveCursorRight(shift_mod);
		return true;
	case GLFW_KEY_END:
		end(shift_mod);
		return true;
	case GLFW_KEY_HOME:
		home(shift_mod);
		return true;
	case GLFW_KEY_TAB:
		type('\t');
		return true;
	case GLFW_KEY_V:
		if (controlMod) {
			const char* clipboardString = glfwGetClipboardString(NULL);
			if (clipboardString) { 
				paste(clipboardString); 
				return true;
			}
		}
		break;
	case GLFW_KEY_C:
		if (controlMod) {
			std::string selectedText = getSelected();
			if (!selectedText.empty()) {
				glfwSetClipboardString(NULL, selectedText.c_str());
				return true;
			};
		}
		break;
	case GLFW_KEY_X:
		if (mods & GLFW_MOD_CONTROL) {
			std::string selectedText = getSelected();
			if (!selectedText.empty()) {
				glfwSetClipboardString(NULL, selectedText.c_str());
				del();
				return true;
			}
		}
		break;
	case GLFW_KEY_A:
		if (controlMod) {
			selectAll();
			return true;
		}
	default:
		return false;
	}
}

bool GraphicalTextArea::charInput(unsigned int codepoint) {
	if (codepoint < 0 || codepoint > 255) return false;
	type(static_cast<char>(codepoint));
	return true;
}

void GraphicalTextArea::draw(ShaderProgram* shader, double delta) {

	// update cursor blink
	cursorBlinkTimer.update(delta);
	if (cursorBlinkTimer.done()) {
		cursorBlinkState = !cursorBlinkState;
		cursorBlinkTimer.restart();
	}

	data.clear();

	// draw margin
	for (size_t line = 0; line < areaSize.y; ++line) {
		std::string numberStr = line < lineCount() ? std::to_string(line + 1) : "";
		for (size_t margin = 0; margin < marginSize; ++margin) {
			size_t index = numberStr.length() - marginSize + margin + 1;
			char c = (index < numberStr.length()) ? numberStr[index] : 0;

			data.push_back({ origin + glm::vec2(margin, line) * charSize, charSize, 2 + 32 * c, 1 });
		}
	}


	// draw text area
	CharPos char_pos(0, 0);
	CharPos cursor_pos = getRealCursor();
	for (char_pos.y = 0; char_pos.y < areaSize.y; ++char_pos.y) {
		for (char_pos.x = 0; char_pos.x < areaSize.x; ++char_pos.x) {
			
			char c = getChar(char_pos);

			bool inverse;
			if (char_pos == cursor_pos) {
				inverse = cursorBlinkState;
			} else {
				inverse = isCharSelected(char_pos);
			}

			data.push_back({ origin + (char_pos.convert<glm::vec2>() + glm::vec2(marginSize, 0)) * charSize, charSize, texture_index + 32 * c, inverse ? 1 : 0 });
		}
	}

	if (data.size() == 0) return;

	shader->use();
	glVertexAttribPointer(shader->a("pos"),	2, GL_FLOAT, false, sizeof(DrawData), &data[0].pos);
	glVertexAttribPointer(shader->a("dim"),	2, GL_FLOAT, false, sizeof(DrawData), &data[0].dim);
	glVertexAttribIPointer(shader->a("tex"), 1, GL_INT, sizeof(DrawData), &data[0].tex);
	glVertexAttribIPointer(shader->a("inv"), 1, GL_INT, sizeof(DrawData), &data[0].invert);

	glDrawArrays(GL_POINTS, 0, data.size());
}
