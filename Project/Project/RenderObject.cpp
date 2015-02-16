#include "RenderObject.hpp"

#define GLM_FORCE_RADIANS

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

// Because of obj loading
#include <fstream>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

renderObject::renderObject()
{
	modelMatrix = glm::mat4(1.0);
}

void renderObject::genBuffer(GLuint shader)
{
	// Read our .obj file
	std::vector< myVec3 > vertices;
	std::vector< myVec2 > uvs; // Not used
	std::vector< myVec3 > normals; // Not used
	std::vector< IndexContainer > indexes;
	
	bool res = loadOBJ("Cone.obj", vertices, uvs, normals, indexes);

	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(myVec3), &vertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(IndexContainer), &indexes[0], GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vArray);
	glBindVertexArray(vArray);

	glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
	
	GLuint vertexPos = glGetAttribLocation(shader, "vertex_position");
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(myVec3), BUFFER_OFFSET(0));

	glFlush();
}

void renderObject::update()
{

	glm::mat4 rotMatrix = glm::mat4(
		cos((glm::pi<float>() / 180)*ry), 0.0f, -sin((glm::pi<float>() / 180)*ry), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin((glm::pi<float>() / 180)*ry), 0.0f, cos((glm::pi<float>() / 180)*ry), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	
	modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -2.0));
	modelMatrix = modelMatrix * rotMatrix;

	ry += 1.0f;

}

const GLfloat * renderObject::getModelMatrix() const
{
	return &modelMatrix[0][0];
}

bool renderObject::loadOBJ(const char * path, std::vector < myVec3 > & out_vertices, std::vector < myVec2 > & out_uvs, std::vector < myVec3 > & out_normals, std::vector <IndexContainer> & out_indexes)
{

	//Open file
	FILE * file;
	fopen_s(&file, path, "r");

	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf_s(file, "%s", lineHeader);
		if (res == EOF) break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0)
		{
			// Push vertices into vector < myVec3 >

			myVec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			out_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			// Push uvs into vector < myVec2 >

			myVec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			out_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			// Push normals into vector < myVec3 >

			myVec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			out_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			// Push faces into vector < IndexContainer >

			std::string vertex1, vertex2, vertex3;
			IndexContainer ic;
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &ic.v1, &ic.vt1, &ic.vn1, &ic.v2, &ic.vt2, &ic.vn2, &ic.v3, &ic.vt3, &ic.vn3);
			if (matches != 9){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			out_indexes.push_back(ic);
		}
	}

	//Succes!
	return true;
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
	glDrawElements(GL_TRIANGLES, 3*12, GL_UNSIGNED_SHORT, (void*)0);


}