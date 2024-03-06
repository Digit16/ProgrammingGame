#include "Game.h"
#include "Callbacks.h"
#include "Utils.h"

void Game::initOpenGL() {

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	int width = 800;
	int height = 800;

	m_window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);

	if (!m_window)
	{
		fprintf(stderr, "Failed to create window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}

void Game::init() {

	glClearColor(0.8f, 0.8f, 0.8f, 1);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glfwSetWindowSizeCallback(m_window, windowResizeCallback);
	glfwSetKeyCallback(m_window, keyCallback);
	glfwSetCharCallback(m_window, charCallback);

	m_sp2d = new ShaderProgram("shaders/2d_vec.glsl", "shaders/2d_geom.glsl", "shaders/2d_frag.glsl");
	m_sp2dPost = new ShaderProgram("shaders/post_vec.glsl", nullptr, "shaders/post_frag.glsl");

	m_asciiTexture = loadTexture("assets/ascii.png");

	m_frameColorBuffer = createColorBuffer(m_window);
	m_frameDepthBuffer = createDepthBuffer(m_window);
	m_frameBuffer = createFrameBuffer(m_frameColorBuffer, m_frameDepthBuffer);

}

void Game::update(double delta) {
	m_textArea.update(delta);

}

void Game::draw() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_sp2d->use();
	glEnableVertexAttribArray(m_sp2d->a("pos"));
	glEnableVertexAttribArray(m_sp2d->a("dim"));
	glEnableVertexAttribArray(m_sp2d->a("tex"));
	glEnableVertexAttribArray(m_sp2d->a("inv"));

	bindTilemap(m_sp2d, m_asciiTexture, 2, glm::ivec2(16, 16));

	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	glUniform2f(m_sp2d->u("window"), width, height);
	glUniform1f(m_sp2d->u("layer"), 0.0f);

	m_textArea.draw(m_sp2d);

	// The value zero is reserved to represent the default framebuffer provided by the windowing system
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	m_sp2dPost->use();
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, m_frameColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUniform1i(m_sp2dPost->u("colorBuffer"), 3);
	glfwSwapBuffers(m_window);
}

void Game::clean() {
	delete m_sp2d;
	delete m_sp2dPost;

	glDeleteFramebuffers(1, &m_frameBuffer);
	glDeleteTextures(1, &m_frameColorBuffer);
	glDeleteRenderbuffers(1, &m_frameDepthBuffer);
}

void Game::cleanOpenGL() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}


