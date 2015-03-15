#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP
#define _CRT_SECURE_NO_DEPRECATE

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <errno.h>
#include <stdio.h>

class renderObject
{
public:
	renderObject();
	virtual ~renderObject();

	void render();

	void genBuffer(GLuint shader);
	void update();

	// ability to get the modelmatrix for objects, these are unique
	const GLfloat * getModelMatrix() const;

	bool loadRawFile(char*);	// Reads a.raw file (Be sure to get size of map first)
	int getHeight(int, int);	// Returns height(0 to 255) of coordinates from a height map

	float setVertexColor(int, int);	// Set color of the map

private:

	bool generated;

	int mapWidth;				// Width of .raw height map
	int mapHeight;				// Height of .raw height map
	int mapSize;				// Total size of height map
	int quadSize;				// Width and height of each QUAD
	unsigned char* g_HeightMap;	// Holds the height map data

	int gridWidth;				// Defined width for actual map
	int gridHeight;				// Defined height for actual map

	struct VertexPosition
	{
		GLfloat x, y, z;
		GLfloat r, g, b;
	};
	VertexPosition* vertices;	// Formerly known as vertexMap* vMap
	GLfloat rgbColor;

	float ry = 0;

	// vArray should be moved out from this class
	// and probably bound to a shader within its own class

	GLuint VBOHeightMap;
	GLuint nrIndex;
	GLuint VAOHeightMap;
	GLuint indexBuffer;

	glm::mat4 modelMatrix;
};

#endif