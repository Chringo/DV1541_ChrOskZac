#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP
#define _CRT_SECURE_NO_DEPRECATE

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <errno.h>
#include <stdio.h>

struct QuadTree
{
	GLfloat x, y;			//Center
	GLfloat size;			//offset
	GLuint q_IndexBuffer;
	GLuint nrIndex;

	QuadTree* topLeft;
	QuadTree* topRight;
	QuadTree* botLeft;
	QuadTree* botRight;

};


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
	QuadTree* quadTree;

	bool generated;

	int mapWidth;				// Width of .raw height map
	int mapHeight;				// Height of .raw height map
	int mapSize;				// Total size of height map
	int quadSize;				// Width and height of each QUAD
	unsigned char* g_HeightMap;	// Holds the height maps raw data

	int gridWidth;				// Defined width for actual map
	int gridHeight;				// Defined height for actual map

	struct VertexPosition
	{
		GLfloat x, y, z;
		GLfloat r, g, b;
	};
	VertexPosition* vertices;	// Holds the colored height map data
	GLfloat rgbColor;

	float ry = 0;
	glm::mat4 modelMatrix;
	
	GLuint VBOHeightMap;
	GLuint VAOHeightMap;
	GLuint indexBuffer;
	GLuint nrIndex;
		
	QuadTree* createQuadTree(int levels, GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY);
	void renderQuadTree(QuadTree* qt);
	void releaseQuadTree(QuadTree* qt);
};

#endif