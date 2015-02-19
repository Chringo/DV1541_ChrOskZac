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
	//std::vector< myVec3 > vertices;
	//std::vector< myVec2 > uvs; // Not used
	//std::vector< myVec3 > normals;
	std::vector< IndexContainer > indexes;
	std::vector < objBuffer > objB;
	
	bool res = loadOBJ("Cone.obj", objB, indexes);

	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, objB.size() * sizeof(objBuffer), &objB[0], GL_DYNAMIC_DRAW);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(IndexContainer), &indexes[0], GL_DYNAMIC_DRAW);
	
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
	
	modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0));
	modelMatrix = modelMatrix * rotMatrix;

	//ry += 1.0f;

}

const GLfloat * renderObject::getModelMatrix() const
{
	return &modelMatrix[0][0];
}

bool renderObject::loadOBJ(const char * path, std::vector < objBuffer > & out_objVec, std::vector <IndexContainer> & out_indexes)
{
	std::vector < myVec3 > vertices;
	std::vector < myVec2 > uvs;
	std::vector < myVec3 > normals;
	//Open file
	FILE * file;
	fopen_s(&file, path, "r");

	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return false;
	}

	fprintf(stdout, "Reading file\n");
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
			// Push faces into vector < IndexContainer >
			IndexContainer ic = {0};
			
			//*************************************************************//
			// .obj needs vt
			//int matches = fscanf_s(file, "%d//%d//%d %d//%d//%d %d//%d//%d\n", &ic.v1, &ic.vt1, &ic.vn1, &ic.v2, &ic.vt2, &ic.vn2, &ic.v3, &ic.vt3, &ic.vn3);
			//*************************************************************//
			
			int _;

			int matches = fscanf_s(file, "%d//%d %d//%d %d//%d\n", &ic.v1, &_, &ic.v2, &_, &ic.v3, &_);
			
			// decrement values in ic once
			int * something = (int*)&ic;
			for (size_t i = 0; i < 3; i++)
			{
				something[i]--;
			}
			
			if (matches != 6){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			out_indexes.push_back(ic);
		}

	}

	fprintf(stdout, "Generating vertices\n");

	for (size_t i = 0; i < vertices.size(); i++)
	{
		objBuffer temp;
		
		for (size_t j = 0; j < 3; j++)
		{
			temp.vertices[j] = vertices[i].pos[j];
			//temp.vns[j] = normals[i].pos[j];
		}
		
		out_objVec.push_back(temp);
	}

	//Succes!
	fclose(file);
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
	glDrawElements(GL_TRIANGLES, indexSize * sizeof(IndexContainer), GL_UNSIGNED_INT, (void*)0);


}