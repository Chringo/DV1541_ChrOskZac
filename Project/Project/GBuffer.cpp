#include "GBuffer.hpp"
#include <stdio.h>
#include <vector>
#include <glm\gtc\matrix_transform.hpp>
#include "Light.hpp"


bool GBuffer::init(int width, int height)
{
	glGenFramebuffers(1, &fbo);
	glGenFramebuffers(1, &lightBuffer.fbo);
	glGenFramebuffers(1, &shadowFbo);

	glGenTextures(1, &depthTexture);
	glGenTextures(1, &diffuseTexture);
	glGenTextures(1, &normalTexture);
	glGenTextures(1, &worldPosTexture);

	glGenTextures(1, &shadowTexture);

	glGenTextures(1, &lightBuffer.lightTexture);

	genQuad();

	frame = 0;
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseTexture, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, normalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, worldPosTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, worldPosTexture, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, DrawBuffers);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "FB error, status: 0x%x\n", Status);
		return false;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// light buffer

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightBuffer.fbo);
	glBindTexture(GL_TEXTURE_2D, lightBuffer.lightTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
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

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// shadow buffer

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowFbo);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	DrawBuffers2[0] = GL_NONE;
	glDrawBuffers(1, DrawBuffers2);

	Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "FB error, status: 0x%x\n", Status);
		return false;
	}


	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// restore default FBO

	this->width = width;
	this->height = height;

	return true;
}

void GBuffer::streamLights(void * data, int nrLigts, int objSize)
{
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer.lightInfo);
	glGenBuffers(1, &lightBuffer.lightInfo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightBuffer.lightInfo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, nrLigts * objSize, NULL, GL_STATIC_COPY);
	
	Light * pdata = (Light*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, nrLigts * objSize, GL_MAP_WRITE_BIT);

	memcpy(pdata, data, nrLigts*objSize);

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	lightBuffer.nrLights = nrLigts;
}

void GBuffer::setProjectionAndView(void * proj, void * view)
{
	lightBuffer.proj = proj;
	lightBuffer.view = view;
}

void GBuffer::setLightView(void* view)
{
	lightVP = view;
}

void GBuffer::draw()
{
	GLuint pos;
	// bind buffer
	glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
	glBindVertexArray(lightVao);

	/// do light calculations

	glBindTexture(GL_TEXTURE_2D, lightBuffer.lightTexture);
	glActiveTexture(GL_TEXTURE0);
	glUseProgram(compShader);

	glBindImageTexture(0, lightBuffer.lightTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(2, diffuseTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(3, normalTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(4, worldPosTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, worldPosTexture);
	*/
	/*pos = glGetUniformLocation(compShader, "diffuse");
	glProgramUniform1i(compShader, pos, 0);
	pos = glGetUniformLocation(compShader, "normal");
	glProgramUniform1i(compShader, pos, 1);
	pos = glGetUniformLocation(compShader, "worldPos");
	glProgramUniform1i(compShader, pos, 2);
	*/

	pos = glGetUniformLocation(compShader, "lightView");
	glUniformMatrix4fv(pos, 1, GL_FALSE, (const GLfloat*)lightVP);
	pos = glGetUniformLocation(compShader, "proj");
	glUniformMatrix4fv(pos, 1, GL_FALSE, (const GLfloat*) lightBuffer.proj);
	pos = glGetUniformLocation(compShader, "view");
	glUniformMatrix4fv(pos, 1, GL_FALSE, (const GLfloat*) lightBuffer.view);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, worldPosTexture);
	pos = glGetUniformLocation(compShader, "worldPosSampler");
	glProgramUniform1i(compShader, pos, 0);

	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	pos = glGetUniformLocation(compShader, "normalSampler");
	glProgramUniform1i(compShader, pos, 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	pos = glGetUniformLocation(compShader, "shadowMap");
	glProgramUniform1i(compShader, pos, 2);

	pos = glGetUniformLocation(compShader, "screensize");
	glProgramUniform2f(compShader, pos, width, height);

	float tx = ceilf((float)width / 32.0f);
	float ty = ceilf((float)height / 32.0f);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer.lightInfo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightBuffer.lightInfo);
	glDispatchCompute(tx, ty , 1);
	
	/*glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightBuffer.fbo);

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(lightShader);

	

	// draw quad, on light FBO
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	*/
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
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);


	pos = glGetUniformLocation(combineShader, "diffuse");
	glProgramUniform1i(combineShader, pos, 0);
	pos = glGetUniformLocation(combineShader, "lightMap");
	glProgramUniform1i(combineShader, pos, 1);
	
	pos = glGetUniformLocation(combineShader, "shadowMap");
	glProgramUniform1i(combineShader, pos, 2);


	// drwa quad, on backbugffer
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	frame++;
}

void GBuffer::bindDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void GBuffer::bindShadow()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowFbo);
}

void GBuffer::bindRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
}

void GBuffer::bindLightRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, lightBuffer.fbo);
}
