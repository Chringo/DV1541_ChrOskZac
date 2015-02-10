#include "GBuffer.hpp"
#include <stdio.h>

bool GBuffer::init(int width, int height)
{
	glGenFramebuffers(1, &fbo);

	glGenTextures(1, &depthTexture);
	glGenTextures(1, &diffuseTexture);
	glGenTextures(1, &normalTexture);
	glGenTextures(1, &worldPosTexture);
	return setTextures(width, height);
}


void GBuffer::update(int width, int height)
{
	setTextures(width, height);
}

bool GBuffer::setTextures(int width, int height)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);


	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseTexture, 0);

	glBindTexture(GL_TEXTURE_2D, normalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);

	glBindTexture(GL_TEXTURE_2D, worldPosTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, worldPosTexture, 0);

	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, DrawBuffers);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "FB error, status: 0x%x\n", Status);
		return false;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

void GBuffer::bindDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void GBuffer::bindRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
}