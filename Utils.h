#pragma once


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "ShaderProgram.h"

#include <functional>
#include <iostream>


GLuint loadTexture(const char* filename);

GLuint createColorBuffer(GLFWwindow* window);

GLuint createDepthBuffer(GLFWwindow* window);

GLuint createFrameBuffer(GLuint color_buffer, GLuint depth_buffer);

void bindTexture(ShaderProgram* shader, GLuint textureId, unsigned textureIndex);

void bindTilemap(ShaderProgram* shader, GLuint textureId, unsigned textureIndex, glm::ivec2 tilemapSize);