#include "RenderObject.hpp"

#define GLM_FORCE_RADIANS

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#define RGB 3
#define RGBA 4

renderObject::renderObject()
{
	modelMatrix = glm::mat4(1.0);
}

void renderObject::genBuffer(GLuint shader)
{
	glDisableVertexAttribArray;

	std::vector< GLuint > indexes;
	std::vector < objBuffer > objB;
	std::string mtlFileName;
	
	std::string fileName = "mustang.obj";
	bool res;
	
	res = loadOBJ(MESH_FOLDER + fileName, mtlFileName, objB, indexes);
	res = loadMTL(MESH_FOLDER + mtlFileName, mtl);
	res = loadTexture(MESH_FOLDER + mtl.texturePath, textureID);
	indexSize = indexes.size();

	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, objB.size() * sizeof(objBuffer), &objB[0], GL_DYNAMIC_DRAW);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vArray);
	glBindVertexArray(vArray);

	GLuint vertexPos = glGetAttribLocation(shader, "vertex_position");
	glEnableVertexAttribArray(vertexPos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objBuffer), BUFFER_OFFSET(0));
	
	GLuint textureNormal = glGetAttribLocation(shader, "texture_normal");
	glEnableVertexAttribArray(textureNormal);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(objBuffer), BUFFER_OFFSET(3 * sizeof(GLfloat)));
	
	GLuint vertexNormal = glGetAttribLocation(shader, "vertex_normal");
	glEnableVertexAttribArray(vertexNormal);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(objBuffer), BUFFER_OFFSET(5 * sizeof(GLfloat)));

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

bool renderObject::loadOBJ(std::string path, std::string & mtlFileName, 
	std::vector < objBuffer > & out_objVec, std::vector <GLuint> & out_indexes)
{
	std::vector < myVec3 > vertices;
	std::vector < myVec2 > uvs;
	std::vector < myVec3 > normals;

	FILE * file;
	fopen_s(&file, path.data(), "r");

	fprintf(stdout, "Reading file\n");

	GLuint index = 0;
	while (1){
		char lineHeader[128];

		// load a row into char array
		char res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));

		// Check for End of file
		if (res == EOF) break;
		
		
		if (strcmp(lineHeader, "mtllib") == 0)
		{
			char mtlName[100];
			fscanf_s(file, "%s", mtlName, sizeof(mtlName));
			
			mtlFileName = std::string(mtlName);
		}
		else if (strcmp(lineHeader, "v") == 0)
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
			struct face
			{
				GLuint v[3];
				GLuint uv[3];
				GLuint vn[3];
			};

			face faceL;
			fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &faceL.v[0], &faceL.uv[0], &faceL.vn[0], &faceL.v[1], &faceL.uv[1], &faceL.vn[1], &faceL.v[2], &faceL.uv[2], &faceL.vn[2]);

			for (GLuint i = 0; i < 3; i++)
			{
				faceL.v[i] --;
				faceL.uv[i] --;
				faceL.vn[i] --;
			}

			// Put the v/vt/vn into the right order and push into vector<objBuffer> 
			for (size_t j = 0; j < 3; j++)
			{
				objBuffer temp;

				// Copying vertices
				temp.vertices[0] = vertices[faceL.v[j]].pos[0];
				temp.vertices[1] = vertices[faceL.v[j]].pos[1];
				temp.vertices[2] = vertices[faceL.v[j]].pos[2];

				// Copying texture normals
				temp.uvs[0] = uvs[faceL.uv[j]].pos[0];
				temp.uvs[1] = uvs[faceL.uv[j]].pos[1];

				// Copying vertex normals
				temp.vns[0] = normals[faceL.vn[j]].pos[0];
				temp.vns[1] = normals[faceL.vn[j]].pos[1];
				temp.vns[2] = normals[faceL.vn[j]].pos[2];

				out_objVec.push_back(temp);

				// push indexes into index buffer :-)
				out_indexes.push_back(index++);
			}
		}
	}

	fprintf(stdout, "Generating vertices\n");

	//Succes!
	fclose(file);
	return true;
}

bool renderObject::loadMTL(std::string path, mtlContainer& mtl)
{
	FILE * file;
	fopen_s(&file, path.data(), "r");

	while (1){
		char lineHeader[128];

		// load a row into char array
		char res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));

		// Check for End Of File
		if (res == EOF) break;

		if (strcmp(lineHeader, "Ns") == 0)
		{
			fscanf_s(file, "%f\n", &mtl.Ns);
		}
		else if (strcmp(lineHeader, "Ka") == 0)
		{
			fscanf_s(file, "%f %f %f\n", &mtl.Ka.pos[0], &mtl.Ka.pos[1], &mtl.Ka.pos[2]);
		}
		else if (strcmp(lineHeader, "Kd") == 0)
		{
			fscanf_s(file, "%f %f %f\n", &mtl.Kd.pos[0], &mtl.Kd.pos[1], &mtl.Kd.pos[2]);
		}
		else if (strcmp(lineHeader, "Ks") == 0)
		{
			fscanf_s(file, "%f %f %f\n", &mtl.Ks.pos[0], &mtl.Ks.pos[1], &mtl.Ks.pos[2]);
		}
		else if (strcmp(lineHeader, "map_Kd") == 0)
		{
			char fileName[100];
			fscanf_s(file, "%s", fileName, sizeof(fileName));
			mtl.texturePath = std::string(fileName);
		}
		/*******************************************************************************/
		// d (ignore)
		/*******************************************************************************/
		/*******************************************************************************/
		// illum (ignore)
		/*******************************************************************************/
	}

	fclose(file);

	// Succes!
	return true;
}

bool renderObject::loadTexture(std::string path, GLuint &ID)
{
	GLint wi;
	GLint he;
	GLint nrOfBytes;
	unsigned char* image = stbi_load(path.data(), &wi, &he, &nrOfBytes, RGB);

	if (image == nullptr)
	{
		std::string toPrint = "Failed to load image " + path;
		fprintf(stdout, toPrint.data());
		return false;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (nrOfBytes == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wi, he, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	else if (nrOfBytes == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wi, he, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}
	else
	{
		fprintf(stdout, "Unexpected number of bytes in texture");
		return false;
	}

	stbi_image_free(image);

	return true;
}

renderObject::~renderObject()
{

}

void renderObject::render(GLuint shader)
{
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBindVertexArray(vArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	if (shader != -1)
	{
		/******************TEXTURE******************************/
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		GLuint pos = glGetUniformLocation(shader, "objTexture");
		glProgramUniform1i(shader, pos, 0);
		/*******************************************************/
	}
	

	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawElements(GL_TRIANGLES, indexSize * sizeof(GLuint), GL_UNSIGNED_INT, (void*)0);
}