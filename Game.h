#pragma once

#include "Callbacks.h"
#include "ShaderProgram.h"
#include "GraphicalTextArea.h"


class Game{

private:
	void initOpenGL();
	void init();
	void update(double delta);
	void draw();
	void clean();
	void cleanOpenGL();

public:
	static Game& getInstance() {
		static Game instance;
		return instance;
	}

	void run() {
		initOpenGL();
		init();

		double lastFrameTime = glfwGetTime();

		while (!glfwWindowShouldClose(m_window))
		{
			double newFrameTime = glfwGetTime();
			double delta = newFrameTime - lastFrameTime;

			update(delta);
			draw();

			lastFrameTime = newFrameTime;
			glfwPollEvents();
		}

		clean();
		cleanOpenGL();
	}

private:
	GLFWwindow* m_window;
	GraphicalTextArea m_textArea{ };

	ShaderProgram* m_sp2d{ nullptr };
	ShaderProgram* m_sp2dPost{ nullptr };

	GLuint m_frameColorBuffer;
	GLuint m_frameDepthBuffer;
	GLuint m_frameBuffer;

	GLuint m_asciiTexture;

	friend void errorCallback(int error, const char* description);
	friend void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void charCallback(GLFWwindow* window, unsigned int codepoint);
	friend void windowResizeCallback(GLFWwindow* window, int width, int height);
};

