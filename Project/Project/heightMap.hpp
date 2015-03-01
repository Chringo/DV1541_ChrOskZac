#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>

#include <GL\glew.h>
#include <GL\GL.h>

class HeightMap
{
public:
	HeightMap();				// Predefined values of a .raw file
	virtual ~HeightMap();

	bool loadRawFile(char*);	// Reads a.raw file (Be sure to get size of map first)
	int getHeight(int, int);	// Returns height(0 to 255) of coordinates from a height map

	void setVertexColor(int, int);	// Set color of the map

	void renderHeightMap();		// Turns heightmap data into primitives and draws them to the screen

private:
	int width;					// Width of .raw height map
	int height;					// Height of .raw height map
	int mapSize;				// Total size of height map
	int quadSize;				// Width and height of each QUAD
	unsigned char* g_HeightMap;	// Holds the height map data

	GLuint dataMap;
};

#endif