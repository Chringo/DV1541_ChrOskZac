#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP
#define _CRT_SECURE_NO_DEPRECATE

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <errno.h>
#include <stdio.h>

struct QuadTree
{
	GLfloat x, y, z = 100;	//Center
	GLfloat size;			//offset
	GLuint q_IndexBuffer;
	GLuint nrIndex;

	bool visible = true;

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
	void update(glm::mat4);

	// ability to get the modelmatrix for objects, these are unique
	const GLfloat * getModelMatrix() const;

	bool loadRawFile(char*);	// Reads a.raw file (Be sure to get size of map first)
	int getHeight(int, int);	// Returns height(0 to 255) of coordinates from a height map

	void createViewFrustum(glm::mat4 proj, glm::mat4 view, glm::vec2 screenSize);

	int renderCount;

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
	GLfloat setVertexColor(int, int);	// Set color of the map

	//float ry = 0;
	glm::mat4 modelMatrix;
	
	glm::vec4 frustumPlanes[6];

	GLuint VBOHeightMap;
	GLuint VAOHeightMap;
	GLuint indexBuffer;
	GLuint nrIndex;

	glm::mat4 view;

	int qLevels;
	void renderQuadTree(QuadTree* qt);
	void releaseQuadTree(QuadTree* qt);
	QuadTree* createQuadTree(int levels, GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY);
	void checkQuadTree(QuadTree* qt, glm::mat4);	

};

#endif