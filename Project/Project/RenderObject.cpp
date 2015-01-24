#include "RenderObject.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

renderObject::renderObject()
{
	generated = false;
}

void renderObject::genBuffer()
{

	const char* vertex_shader = R"(
		#version 400
		layout(location = 0) in vec3 vertex_position;
		layout(location = 1) in vec3 vertex_color;
		
		out vec3 color;
		
		void main () {
			color = vertex_color;
			gl_Position = vec4 (vertex_position, 1.0);
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
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);

	//create fragment shader
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);


	//link shader program (connect vs and ps)
	gShaderProgram = glCreateProgram();
	glAttachShader(gShaderProgram, fs);
	glAttachShader(gShaderProgram, vs);
	glLinkProgram(gShaderProgram);

	glDeleteShader(vs);
	glDeleteShader(fs);


	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);


	struct TriangleVertex
	{
		float x, y, z;
		float r, g, b;
	};

	TriangleVertex tri[1];

	glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vArray);
	glBindVertexArray(vArray);

	glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
	glEnableVertexAttribArray(1);


	GLuint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));
	GLuint vertexColor = glGetAttribLocation(gShaderProgram, "vertex_color");
	glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));

	generated = true;
}

void renderObject::fillBuffer()
{
	if (generated)
	{
		struct TriangleVertex
		{
			float x, y, z;
			float r, g, b;
		};

		TriangleVertex tri[4] =
		{

			0.5f, 0.5f, 0.0f,	//v0 pos
			1.0f, 1.0f, 1.0f, // normal stuff

			0.5f, -0.5f, 0.0f,	//v1
			0.0f, 0.0f, 1.0f,

			-0.5f, 0.5f, 0.0f, //v2
			0.0f, 1.0f, 0.0f,

			-0.5f, -0.5f, 0.0f, //v3
			1.0f, 0.0f, 0.0f
		};
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_DYNAMIC_DRAW);

		glFlush();
	}
}

renderObject::~renderObject()
{

}

void renderObject::render()
{
	glUseProgram(gShaderProgram);
	glBindVertexArray(vArray);

	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}