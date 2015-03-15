#include "Scene.hpp"
#include <iostream>
#include <sstream>
#include <windows.h>
#include "ReadShader.hpp"
#include "Light.hpp"

scene::scene()
{
	fboShader = 0;
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

	gBuffer.setProjectionAndView(&cam.projection, &viewMatrix);

	GLuint nr;

	Light * l = readLights("scene.light", nr);

	gBuffer.streamLights(l, nr, sizeof(Light));

	delete l;

	obj.genBuffer(fboShader);

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
	glUseProgram(0);
	frameUpdate();

	gBuffer.bindDraw();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	glUseProgram(fboShader);

	GLuint modelMat = glGetUniformLocation(fboShader, "model");
	GLuint viewMat = glGetUniformLocation(fboShader, "view");
	GLuint projectionMat = glGetUniformLocation(fboShader, "projection");

	const GLfloat * modelMatrix = obj.getModelMatrix();

	glUniformMatrix4fv(modelMat, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMat, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMat, 1, GL_FALSE, &projectionMatrix[0][0]);

	obj.render(fboShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gBuffer.draw();

	gBuffer.bindRead();
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, (GLint)cam.width, (GLint)cam.height, 0, 0, (GLint)cam.width/5, (GLint)cam.height/5, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, (GLint)cam.width, (GLint)cam.height, (GLint)cam.width / 5, 0, 2*(GLint)cam.width / 5, (GLint)cam.height / 5, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glBlitFramebuffer(0, 0, (GLint)cam.width, (GLint)cam.height, 2*(GLint)cam.width / 5, 0, 3 * (GLint)cam.width / 5, (GLint)cam.height / 5, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	gBuffer.bindLightRead();

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, (GLint)cam.width, (GLint)cam.height, 3 * (GLint)cam.width / 5, 0, 4 * (GLint)cam.width / 5, (GLint)cam.height / 5, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	
}

camera &scene::getCamera()
{
	return cam;
}

// generate the shader
void scene::generateShader()
{
	std::string vboShaders[] = {"shaders/fbo_vs.glsl", "shaders/fbo_gs.glsl", "shaders/fbo_fs.glsl"};
	GLenum vboShaderType[] = { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };

	GLuint program = 0;

	if (CreateProgram(program, vboShaders, vboShaderType, 3))
	{
		if (fboShader != 0)
		{
			glDeleteProgram(fboShader);
		}
		fboShader = program;
	}

	std::string quadShaders[] = { "shaders/quad_vs.glsl", "shaders/quad_fs.glsl" };
	GLenum quadShaderType[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

	program = 0;

	if (CreateProgram(program, quadShaders, quadShaderType, 2))
	{
		if (gBuffer.combineShader != 0)
		{
			glDeleteProgram(gBuffer.combineShader);
		}
		gBuffer.combineShader = program;
	}

	std::string lightShader = "shaders/light_cs.glsl";
	GLenum lightShaderType = GL_COMPUTE_SHADER;

	program = 0;

	if (CreateProgram(program, &lightShader, &lightShaderType, 1))
	{
		if (gBuffer.lightShader != 0)
		{
			glDeleteProgram(gBuffer.compShader);
		}
		gBuffer.compShader = program;
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
		projectionMatrix = glm::perspective(glm::pi<float>()* 0.45f, cam.width / cam.height, 0.01f, 100.0f);
		cam.projection = projectionMatrix;
		updateGBuffer = false;
	}

	if (reloadShader)
	{
		generateShader();
		Sleep(200);
		reloadShader = false;
	}
	
	// update camera and projection matrix
	viewMatrix = glm::mat4(cam.rot) * cam.translation;
	gBuffer.setProjectionAndView(&cam.projection, &viewMatrix);
}