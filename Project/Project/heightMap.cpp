#include "HeightMap.hpp"

#include <errno.h>

HeightMap::HeightMap()
{
	width = 1024;
	height = 1024;
	mapSize = width * height;
	quadSize = 4;
	g_HeightMap = new unsigned char[mapSize];
}
HeightMap::~HeightMap()
{
	delete g_HeightMap;
}

bool HeightMap::loadRawFile(char* fileName)
{
	bool loadFromFile = false;
	FILE* file = nullptr;

	// Opens the file in Read/Binary mode.
	file = fopen(fileName, "rb");

	// Check if file was found and could open it
	if (file != nullptr)
	{
		// Read the .raw file into data array.
		fread(g_HeightMap, 1, mapSize, file);

		// Check for data error
		int result = ferror(file);
		result = errno;
		if (result)
		{
			// Reading failed - check errno.h
			// https://msdn.microsoft.com/en-us/library/t3ayayh1.aspx
			loadFromFile = false;
		}
		else
		{
			loadFromFile = true;
		}
	}
	else
	{
		// Height map not found
		loadFromFile = false;
	}
	fclose(file);
	return loadFromFile;
}
int HeightMap::getHeight(int _x, int _y)
{
	// Force x and y to cap out at (mapSize - 1)
	int x = _x % width;
	int y = _y % height;

	// Check if empty
	if (!g_HeightMap)
	{
		// TODO: printf to console
		return 0;
	}

	// Treat the array like a 2D array (.raw format is a single array)
	return g_HeightMap[x + (y * height)];	// Index into our height array and return the height
}

void HeightMap::setVertexColor(int x, int y)
{
	// Check if empty
	if (!g_HeightMap)
	{
		// TODO: printf to console
		return;
	}

	// Set color for a vertex based on height index.
	// Ratio of color: from 0 to 1.0 by dividing the height by 256.0
	float fColor = -0.15f + (getHeight(x, y) / 256.0f);
	//float fColor = 1.0;

	// Assign shade to the current vertex
	// glColor3f(fColor, fColor, fColor);
}

void HeightMap::renderHeightMap()
{
	if (!g_HeightMap)
	{
		// TODO: printf to console
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 256*256);		// glBegin( GL_QUADS ); - replaced?

	glGenBuffers(1, &dataMap);
	glBindBuffer(GL_ARRAY_BUFFER, dataMap);
	struct vertexMap
	{
		float x, y, z;
		float r, g, b;
	};
	const int i = 1024 / 4;
	vertexMap* vMap = new vertexMap[i*i];

	int x, y, z;		// For readability/visuality (Where the map is in the dimension)
	int vCount = 0;
	for (int _w = 0; _w < width / 4; _w += quadSize) //_width of map
	{
		for (int _h = 0; _h < height / 4; _h += quadSize) //_height of map
		{
			// Get (X, Y, Z) value for bottom left vertex
			x = _w;
			y = getHeight(_w * 4, _h * 4);
			z = _h;
			vMap[vCount] = vertexMap{ x, y, z, 0, 0, 0 };
			vCount++;

			// Get (X, Y, Z) value for top left vertex
			x = _w;
			y = getHeight(_w * 4, _h * 4 + quadSize);
			z = _h + quadSize;
			vMap[vCount] = vertexMap{ x, y, z, 0, 0, 0 };
			vCount++;

			// Get (X, Y, Z) value for top right vertex
			x = _w + quadSize;
			y = getHeight(_w * 4 + quadSize, _h * 4 + quadSize);
			z = _h + quadSize;
			vMap[vCount] = vertexMap{ x, y, z, 0, 0, 0 };
			vCount++;

			// Get (X, Y, Z) value for bottom right vertex
			x = _w + quadSize;
			y = getHeight(_w * 4 + quadSize, _h * 4);
			z = _h;
			vMap[vCount] = vertexMap{ x, y, z, 0, 0, 0 };
			vCount++;
		}
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(vMap), &vMap, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete vMap;
	glFlush();
}