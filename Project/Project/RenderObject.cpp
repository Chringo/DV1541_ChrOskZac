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
	std::vector< GLuint > indexes;
	std::vector < objBuffer > objB;
	
	bool res = loadOBJ("Meshes/Cube.obj", objB, indexes);

	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, objB.size() * sizeof(objBuffer), &objB[0], GL_DYNAMIC_DRAW);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_DYNAMIC_DRAW);
	
	// fauling haccus
	indexSize = indexes.size();

	glGenVertexArrays(1, &vArray);
	glBindVertexArray(vArray);

	glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
	glEnableVertexAttribArray(1);
	
	GLuint vertexPos = glGetAttribLocation(shader, "vertex_position");
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(objBuffer), BUFFER_OFFSET(0));
	GLuint vertexNormal = glGetAttribLocation(shader, "vertex_normal");
	glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(objBuffer), BUFFER_OFFSET(3 * sizeof(GLfloat)));

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

bool renderObject::loadOBJ(const char * path, std::vector < objBuffer > & out_objVec, std::vector <GLuint> & out_indexes)
{
	std::vector < myVec3 > vertices;
	std::vector < myVec2 > uvs;
	std::vector < myVec3 > normals;
	std::vector < std::string > faces;

	FILE * file;
	fopen_s(&file, path, "r");

	while (1){

		char lineHeader[128];
		// read the first word of the line
		char res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF) break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0)
		{
			// Push vertices into vector < myVec3 >

			myVec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.pos[0], &vertex.pos[1], &vertex.pos[2]);
			vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			// Push uvs into vector < myVec2 >

			myVec2 uv;
			fscanf_s(file, "%f %f\n", &uv.pos[0], &uv.pos[1]);
			uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			// Push normals into vector < myVec3 >

			myVec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.pos[0], &normal.pos[1], &normal.pos[2]);
			normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			// read faces as char array and store in vector < string >
			char s1[15];
			char s2[15];
			char s3[15];

			fscanf_s(file, "%s %s %s", s1, sizeof(s1), s2, sizeof(s2), s3, sizeof(s3));

			faces.push_back(std::string(s1));
			faces.push_back(std::string(s2));
			faces.push_back(std::string(s3));
		}

	}

	GLuint newIndex = 0;
	for (size_t i = 0; i < faces.size(); i++)
	{
		bool stringFound = false;
		for (size_t j = 0; j < out_indexes.size() && !stringFound; j++)
		{
			// If the string already have been indexed earlier
			if (faces[i] == faces[j])
			{
				out_indexes.push_back(out_indexes[j]);
				stringFound = true;
				break;
			}
		}
		
		// If the string havn´t been indexed, index it
		if (!stringFound)
		{
			out_indexes.push_back(newIndex);
			newIndex++;
		}
	}

	for (size_t i = 0; i < vertices.size(); i++)
	{
		objBuffer temp;
		
		for (size_t j = 0; j < 3; j++)
		{
			temp.vertices[j] = vertices[i].pos[j];
			temp.vns[j] = normals[i].pos[j];
		}

		//
		// uv:s here
		//
		
		out_objVec.push_back(temp);
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
	glDrawElements(GL_TRIANGLES, indexSize * sizeof(GLuint), GL_UNSIGNED_INT, (void*)0);


}