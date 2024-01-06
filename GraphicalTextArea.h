#pragma once
#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "TextArea.h"
#include "Timer.h"


struct DrawData {
	glm::vec2 pos;
	glm::vec2 dim;
	int tex;
	int invert; // treated as bool
};

class GraphicalTextArea : public TextArea {
public:
	using TextArea::TextArea;

	bool keyInput(int key, int scancode, int action, int mods);
	bool charInput(unsigned int codepoint);

	void draw(ShaderProgram* shader, double delta);

private:
	std::vector<DrawData> data;

	glm::vec2 origin{ 48, 80 };
	glm::ivec2 areaSize{ 40, 20 };
	glm::vec2 charSize{ 16, 32 };

	size_t marginSize = 4;
	
	int texture_index = 2;

	Timer cursorBlinkTimer{ 0.5 };
	bool cursorBlinkState = false;
};