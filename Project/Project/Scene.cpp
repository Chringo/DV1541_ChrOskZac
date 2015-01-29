#include "Scene.hpp"

scene::scene()
{

}

scene::~scene()
{

}

// create shader and object buffers
void scene::requestBuffer()
{
	generateShader();
	obj.genBuffer(shaderProgram);
}

// update the objects and camera when that is implemented
void scene::updateScene()
{
	obj.update();
}

// render our stuff
void scene::renderScene()
{
	frameUpdate();

	glUseProgram(shaderProgram);

	GLuint modelMat = glGetUniformLocation(shaderProgram, "model");
	GLuint viewMat = glGetUniformLocation(shaderProgram, "view");
	GLuint projectionMat = glGetUniformLocation(shaderProgram, "projection");

	const GLfloat * modelMatrix = obj.getModelMatrix();

	glUniformMatrix4fv(modelMat, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMat, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMat, 1, GL_FALSE, &projectionMatrix[0][0]);

	obj.render();
}

camera &scene::getCamera()
{
	return cam;
}

// generate the shader
void scene::generateShader()
{
	const char* vertex_shader = R"(
		#version 400
		layout(location = 0) in vec3 vertex_position;
		layout(location = 1) in vec3 vertex_color;
		
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;


		out vec3 color;
		
		void main () {
			color = vertex_color;
			gl_Position = projection * view * model * vec4 (vertex_position, 1.0);
		}
	)";

	const char* fragment_shader = R"(
		#version 400
		in vec3 color;
		out vec4 fragment_color;
		void main () {
			fragment_color = vec4 (color, 1.0);
		}
	)";

	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);

	//create fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);


	//link shader program (connect vs and ps)
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);

	glDeleteShader(vs);
	glDeleteShader(fs);

}

// run this once per frame only
// used for updating data that only needs one change per frame
void scene::frameUpdate()
{

	// update camera
	
	viewMatrix = glm::mat4(cam.rot) * cam.translation;

	projectionMatrix = glm::perspective(glm::pi<float>()* 0.45f, cam.width / cam.height, 0.5f, 100.0f);

	// register new completed objects
}