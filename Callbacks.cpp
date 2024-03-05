#include "Callbacks.h"
#include <cstdio>

#include "Game.h"


void errorCallback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Game& game = Game::getInstance();
	game.m_textArea.keyInput(key, scancode, action, mods);
}

void charCallback(GLFWwindow* window, unsigned int codepoint) {
	Game& game = Game::getInstance();
	game.m_textArea.charInput(codepoint);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	glViewport(0, 0, width, height);
}

