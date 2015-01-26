#include "RenderObject.hpp"

#define GLM_FORCE_RADIANS

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

renderObject::renderObject()
{
	modelMatrix = glm::mat4(1.0);

	generated = false;
}

void renderObject::genBuffer(GLuint shader)
{

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



	/// this should be moved out from this class
	/// as it is bound to shader, and can be used across multiple objects
	GLuint vertexPos = glGetAttribLocation(shader, "vertex_position");
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));
	GLuint vertexColor = glGetAttribLocation(shader, "vertex_color");
	glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
	/// 

	generated = true;
}

void renderObject::update()
{

	modelMatrix = glm::mat4(
		cos((glm::pi<float>() / 180)*ry), 0.0f, -sin((glm::pi<float>() / 180)*ry), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin((glm::pi<float>() / 180)*ry), 0.0f, cos((glm::pi<float>() / 180)*ry), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	ry += 1.0f;

	fillBuffer();
}

const GLfloat * renderObject::getModelMatrix() const
{
	return &modelMatrix[0][0];
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
			GLshort v1, v2, v3;
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
	
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBindVertexArray(vArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);


}