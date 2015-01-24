#include "RenderObject.hpp"

#define GLM_FORCE_RADIANS

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

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

	struct TriangleVertex
	{
		float x, y, z;
		float r, g, b;
	};

	TriangleVertex tri[1];

	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tri), tri, GL_DYNAMIC_DRAW);

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

		struct Ind
		{
			GLshort x, y, z;
		};

		Ind index[2] =
		{
			0, 1, 2,
			1, 2, 3
		};

		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_DYNAMIC_DRAW);

		glFlush();
	}
}

renderObject::~renderObject()
{

}

void renderObject::render()
{
	glUseProgram(gShaderProgram);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBindVertexArray(vArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);


	ry += 1.0f;

	glm::mat4 modelMatrix = glm::mat4(
		cos((glm::pi<float>() / 180)*ry), 0.0f, -sin((glm::pi<float>() / 180)*ry), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin((glm::pi<float>() / 180)*ry), 0.0f, cos((glm::pi<float>() / 180)*ry), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 viewMatrix = glm::lookAt(
		glm::vec3(0, 0, 2),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
		);

	glm::mat4 projectionMatrix = glm::perspective(glm::pi<float>() * 0.45f, 640.0f / 480.0f, 0.5f, 20.0f);

	GLuint modelMat = glGetUniformLocation(gShaderProgram, "model");
	GLuint viewMat = glGetUniformLocation(gShaderProgram, "view");
	GLuint projectionMat = glGetUniformLocation(gShaderProgram, "projection");

	glUniformMatrix4fv(modelMat, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(viewMat, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMat, 1, GL_FALSE, &projectionMatrix[0][0]);

}