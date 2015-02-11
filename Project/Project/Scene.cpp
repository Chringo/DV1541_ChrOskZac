#include "Scene.hpp"
#include <iostream>
#include <sstream>
#include <windows.h>
#include "ReadShader.hpp"


scene::scene()
{
	shaderProgram = 0;
}

scene::~scene()
{

}

// create shader and object buffers
bool scene::requestBuffer(int width, int height)
{

	if (gBuffer.init(400, 400))
	{
		fprintf(stdout, "Created framebuffer\n");
	}

	generateShader();
	obj.genBuffer(shaderProgram);

	return true;
}

// update the objects
void scene::updateScene()
{
	obj.update();
}

// render our stuff
void scene::renderScene()
{
	frameUpdate();

	gBuffer.bindDraw();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	

	glUseProgram(shaderProgram);

	GLuint modelMat = glGetUniformLocation(shaderProgram, "model");
	GLuint viewMat = glGetUniformLocation(shaderProgram, "view");
	GLuint projectionMat = glGetUniformLocation(shaderProgram, "projection");

	const GLfloat * modelMatrix = obj.getModelMatrix();

	glUniformMatrix4fv(modelMat, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMat, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMat, 1, GL_FALSE, &projectionMatrix[0][0]);

	obj.render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gBuffer.bindRead();
	
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, (GLint)cam.width, (GLint)cam.height, 0, 0, (GLint)cam.width, (GLint)cam.height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, (GLint)cam.width, (GLint)cam.height, 0, 0, (GLint)cam.width/5, (GLint)cam.height/5, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glBlitFramebuffer(0, 0, (GLint)cam.width, (GLint)cam.height, (GLint)cam.width / 5, 0, 2*(GLint)cam.width / 5, (GLint)cam.height / 5, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

camera &scene::getCamera()
{
	return cam;
}

// generate the shader
void scene::generateShader()
{
	std::string shaders[] = {"shaders/fbo_vs.glsl", "shaders/fbo_gs.glsl", "shaders/fbo_fs.glsl"};
	GLenum shaderType[] = { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };

	GLuint program = 0;

	if (CreateProgram(program, shaders, shaderType, 3))
	{
		if (shaderProgram != 0)
		{
			glDeleteProgram(shaderProgram);
		}
		shaderProgram = program;
	}
}

void scene::screenChanged()
{
	updateGBuffer = true;
}

void scene::queueReloadShader()
{
	reloadShader = true;
}

// run this once per frame only
// used for updating data that only needs one change per frame
void scene::frameUpdate()
{
	// update the gbuffer size
	// only needed if the window is resized
	if (updateGBuffer)
	{
		gBuffer.update((int)cam.width, (int)cam.height);
		updateGBuffer = false;
	}

	if (reloadShader)
	{
		generateShader();
		Sleep(200);
		reloadShader = false;
	}
	// update camera
	
	viewMatrix = glm::mat4(cam.rot) * cam.translation;

	projectionMatrix = glm::perspective(glm::pi<float>()* 0.45f, cam.width / cam.height, 0.01f, 100.0f);

	// register new completed objects
}