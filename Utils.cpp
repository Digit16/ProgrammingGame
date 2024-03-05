#include "Utils.h"
#include <lodepng.h>

#include "ShaderProgram.h"


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