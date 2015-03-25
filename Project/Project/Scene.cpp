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
	generateShader();

	if (gBuffer.init(400, 400))
	{
		fprintf(stdout, "Created framebuffer\n");
	}

	obj.loadRawFile("terrain.raw");
	obj.genBuffer(shaderProgram);
	return true;
}

// update the objects
void scene::updateScene()
{
	obj.update(cam.translation);
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

	
	gBuffer.draw();
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

	std::string shader[] = { "shaders/quad_vs.glsl", "shaders/quad_fs.glsl" };
	GLenum shadeType[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

	program = 0;

	if (CreateProgram(program, shader, shadeType, 2))
	{
		if (gBuffer.combineShader != 0)
		{
			glDeleteProgram(gBuffer.combineShader);
		}
		gBuffer.combineShader = program;
	}

	shader[1] = "shaders/light_fs.glsl";

	program = 0;

	if (CreateProgram(program, shader, shadeType, 2))
	{
		if (gBuffer.lightShader != 0)
		{
			glDeleteProgram(gBuffer.lightShader);
		}
		gBuffer.lightShader = program;
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
	projectionMatrix = glm::perspective(glm::pi<float>()* 0.45f, (cam.width / cam.height), 0.1f, 1000.0f);
	obj.createViewFrustum(projectionMatrix, viewMatrix, glm::vec2(cam.width, cam.height));

}