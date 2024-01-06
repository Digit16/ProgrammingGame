#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "lodepng.h"

#include "LoadOBJ.h"
#include "ShaderProgram.h"
#include "Timer.h"
#include "GraphicalTextArea.h"


ShaderProgram* sp2D;
ShaderProgram* spPost;

GLuint frameColorBuffer;
GLuint frameDepthBuffer;
GLuint frameBuffer;

GLuint asciiTexture;

GraphicalTextArea textArea("The quick brown\nfox jumps over\nthe lazy dog");


GLuint createColorBuffer(GLFWwindow* window) {

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	unsigned int colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

	return colorBuffer;
}

GLuint createDepthBuffer(GLFWwindow* window) {

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	unsigned int depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	return depthBuffer;
}

GLuint createFrameBuffer(GLuint color_buffer, GLuint depth_buffer) {
	GLuint frameBuffer;

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, color_buffer, 0);

	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER, depth_buffer);

	return frameBuffer;
}

void errorCallback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	textArea.keyInput(key, scancode, action, mods);
}

void charCallback(GLFWwindow* window, unsigned int codepoint) {
	textArea.charInput(codepoint);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    glViewport(0, 0, width, height);
}

GLuint loadTexture(const char* filename) {
	GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    std::vector<unsigned char> image;
    unsigned width, height;

    unsigned error = lodepng::decode(image, width, height, filename); // TODO: handle error

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return tex;
}

GLuint createColor(glm::vec3 rgb, GLuint textureId=-1) {

	unsigned char image[] = {
		(unsigned char)(rgb.r * 255.0f),
		(unsigned char)(rgb.g * 255.0f),
		(unsigned char)(rgb.b * 255.0f)
	};

	if (textureId == -1) glGenTextures(1, &textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);

	GLint mipmapLevel = 0;
	GLint componentNumber = 4; // rgba
	
	GLint width = 1;
	GLint height = 1;

	glTexImage2D(GL_TEXTURE_2D, mipmapLevel, componentNumber, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	
	return textureId;
}

void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0.8f,0.8f,0.8f,1);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCharCallback(window, charCallback);

	sp2D = new ShaderProgram("shaders/v_2d.glsl", "shaders/g_2d.glsl", "shaders/f_2d.glsl");
	spPost = new ShaderProgram("shaders/v_post.glsl", nullptr, "shaders/f_post.glsl");

	asciiTexture = loadTexture("assets/ascii.png");

	frameColorBuffer = createColorBuffer(window);
	frameDepthBuffer = createDepthBuffer(window);
	frameBuffer = createFrameBuffer(frameColorBuffer, frameDepthBuffer);

}

void freeOpenGLProgram(GLFWwindow* window) {
    delete sp2D;
	delete spPost;

	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &frameColorBuffer);
	glDeleteRenderbuffers(1, &frameDepthBuffer);
}

void bindTexture(ShaderProgram* shader, GLuint textureId, unsigned textureIndex) {
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, textureId);
	std::string address = "textures[" + std::to_string(textureIndex) + "]";
	glUniform1i(shader->u(address.c_str()), textureIndex);

}

void bindTilemap(ShaderProgram* shader, GLuint textureId, unsigned textureIndex, glm::ivec2 tilemapSize) {
	bindTexture(shader, textureId, textureIndex);
	std::string tilemapSizeAddress = "tilemapSize[" + std::to_string(textureIndex) + "]";
	glUniform2i(shader->u(tilemapSizeAddress.c_str()), tilemapSize.x, tilemapSize.y);
}

void drawScene(GLFWwindow* window, double delta) {

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	sp2D->use();
	glEnableVertexAttribArray(sp2D->a("pos"));
	glEnableVertexAttribArray(sp2D->a("dim"));
	glEnableVertexAttribArray(sp2D->a("tex"));
	glEnableVertexAttribArray(sp2D->a("inv"));

	bindTilemap(sp2D, asciiTexture, 2, glm::ivec2(16, 16));

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glUniform2f(sp2D->u("window"), width, height);
	glUniform1f(sp2D->u("layer"), 0.0f);

	textArea.draw(sp2D, delta);

	// The value zero is reserved to represent the default framebuffer provided by the windowing system
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	spPost->use();
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, frameColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUniform1i(spPost->u("colorBuffer"), 3);
    glfwSwapBuffers(window); 
}


int main(void)
{
	GLFWwindow* window; 

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) { 
		fprintf(stderr, "Failed to initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}
	
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	int width = 800;
	int height = 800;

	window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Failed to create window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); 
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window);

	double lastFrameTime = glfwGetTime();


	while (!glfwWindowShouldClose(window))
	{
		double newFrameTime = glfwGetTime();
		double delta = newFrameTime - lastFrameTime;
		std::string fpsText = std::to_string(1 / delta);

		glfwSetWindowTitle(window, fpsText.c_str());

		drawScene(window, delta);

		lastFrameTime = newFrameTime;

		glfwPollEvents();
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

