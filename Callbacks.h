#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>


void errorCallback(int error, const char* description);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void charCallback(GLFWwindow* window, unsigned int codepoint);

void windowResizeCallback(GLFWwindow* window, int width, int height);