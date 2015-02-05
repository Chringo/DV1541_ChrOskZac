#include "Scene.hpp"
#include <iostream>
#include <sstream>
#include <windows.h>

void CheckShader(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		std::stringstream ss;
		for (int i = 0; i < maxLength; i++)
		{
			ss << errorLog[i];
		}
		std::cout << ss.str() << std::endl;
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.
		return;
	}
}


scene::scene()
{

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
	
	glBlitFramebuffer(0, 0, (GLint)cam.width, (GLint)cam.height, 0, 0, (GLint)cam.width/2, (GLint)cam.height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	
}

camera &scene::getCamera()
{
	return cam;
}

// generate the shader
void scene::generateShader()
{
	const char* vertex_shader = R"(
		#version 430
		layout(location = 0) in vec3 vertex_position;
		layout(location = 1) in vec3 vertex_color;
		
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		out vec3 colorG;
		
		void main () {
			colorG = vertex_color;
			gl_Position = projection * view * model * vec4 (vertex_position, 1.0);
		}
	)";

	const char* geometry_shader = R"(
		#version 430
		layout (triangles) in;
		layout (triangle_strip) out;
		layout (max_vertices = 3) out;
		in vec3 colorG[];

		out vec3 color;
		out vec3 normal;

		void main () {
			normal = normalize( cross( vec3( gl_in[1].gl_Position - gl_in[0].gl_Position ), vec3( gl_in[2].gl_Position - gl_in[0].gl_Position) ) );

			for( int i = 0; i < 3; i++ )
			{
				gl_Position = gl_in[i].gl_Position;
				color = colorG[i];
				EmitVertex();
			}
			EndPrimitive();
		}
	)";

	const char* fragment_shader = R"(
		#version 430
		in vec3 color;
		in vec3 normal;
		//out vec4 fragment_color;

		layout (location = 0) out vec3 diffuseOut; 

		void main () {
			//fragment_color = vec4 (color, 1.0);
			//diffuseOut = color;
			diffuseOut = normalize(normal);
		}
	)";

	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);
	CheckShader(vs);

	//debug info regarding vertex shader compilation
	GLint vertex_compiled;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &vertex_compiled);
	if (vertex_compiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(vs, 1024, &log_length, message);
		OutputDebugStringA(message);
	}

	//create vertex shader
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, &geometry_shader, nullptr);
	glCompileShader(gs);
	CheckShader(gs);

	//debug info regarding geometry shader compilation
	GLint geometry_compiled;
	glGetShaderiv(gs, GL_COMPILE_STATUS, &geometry_compiled);
	if (geometry_compiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(gs, 1024, &log_length, message);
		OutputDebugStringA(message);
	}

	//create fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);
	CheckShader(fs);

	//debuf info regarding fragment shader compilation
	GLint fragment_compiled;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &fragment_compiled);
	if (fragment_compiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(fs, 1024, &log_length, message);
		OutputDebugStringA(message);
	}

	//link shader program (connect vs and ps)
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, gs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);

	//debug info regarding linking
	GLint linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetProgramInfoLog(shaderProgram, 1024, &log_length, message);
		OutputDebugStringA(message);
	}

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);
}

void scene::screenChanged()
{
	updateGBuffer = true;
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
	// update camera
	
	viewMatrix = glm::mat4(cam.rot) * cam.translation;

	projectionMatrix = glm::perspective(glm::pi<float>()* 0.45f, cam.width / cam.height, 0.01f, 100.0f);

	// register new completed objects
}