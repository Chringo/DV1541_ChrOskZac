#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP


#include <GL\glew.h>

class heightMap
{
public:
	heightMap();
	virtual ~heightMap();

	bool loadDataMap();

private:
	int width;
	int height;
	GLubyte g_HeightMap[];
};

#endif