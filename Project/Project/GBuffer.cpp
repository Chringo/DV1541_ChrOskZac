#include "GBuffer.hpp"
#include <stdio.h>

bool GBuffer::init(int width, int height)
{
	glGenFramebuffers(1, &fbo);
	glGenFramebuffers(1, &lightBuffer.fbo);

	glGenTextures(1, &depthTexture);
	glGenTextures(1, &diffuseTexture);
	glGenTextures(1, &normalTexture);
	glGenTextures(1, &worldPosTexture);
	glGenTextures(1, &lightBuffer.lightTexture);

	genQuad();
	
	return setTextures(width, height);
}


void GBuffer::update(int width, int height)
{
	setTextures(width, height);
}

void GBuffer::genQuad()
{
	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	};

	TriangleVertex tri[4] = 
	{
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, -1.0f, 0.0f,
		1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f,
		0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f,
		0.0f, 0.0f
	};

	glGenBuffers(1, &lightVbo);
	glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);

	glGenVertexArrays(1, &lightVao);
	glBindVertexArray(lightVao);

	glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
	glEnableVertexAttribArray(1);

	/// this should be moved out from this class
	/// as it is bound to shader, and can be used across multiple objects
	//GLuint vertexPos = glGetAttribLocation(lightShader, "vertex_position");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), (void*) 0);
	//GLuint uv = glGetAttribLocation(lightShader, "vertex_texCoord");
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), (void*)(sizeof(float)*3));
}

bool GBuffer::setTextures(int width, int height)
{
	if (width == 0)
	{
		width = 1;
	}
	if (height == 0)
	{
		height = 1;
	}
	// gbuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseTexture, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, normalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, worldPosTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, worldPosTexture, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

	// light buffer

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightBuffer.fbo);
	glBindTexture(GL_TEXTURE_2D, lightBuffer.lightTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightBuffer.lightTexture, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLenum DrawBuffers2[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers2);

	Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "FB error, status: 0x%x\n", Status);
		return false;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

void GBuffer::draw()
{
	// bind buffer
	glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
	glBindVertexArray(lightVao);

	/// do light calculations
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightBuffer.fbo);

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(lightShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, worldPosTexture);

	GLuint pos = glGetUniformLocation(lightShader, "diffuse");
	glProgramUniform1i(lightShader, pos, 0);
	pos = glGetUniformLocation(lightShader, "normal");
	glProgramUniform1i(lightShader, pos, 1);
	pos = glGetUniformLocation(lightShader, "worldPos");
	glProgramUniform1i(lightShader, pos, 2);

	// draw quad, on light FBO
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	/// do end result
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glUseProgram(combineShader);
	glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
	glBindVertexArray(lightVao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, lightBuffer.lightTexture);

	pos = glGetUniformLocation(combineShader, "diffuse");
	glProgramUniform1i(combineShader, pos, 0);
	pos = glGetUniformLocation(combineShader, "lightMap");
	glProgramUniform1i(combineShader, pos, 1);
	
	// draw quad, on backbuffer
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::bindDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void GBuffer::bindRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
}

void GBuffer::bindLightRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, lightBuffer.fbo);
}