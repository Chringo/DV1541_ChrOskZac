#include "heightMap.hpp"


heightMap::heightMap()
{
	width = 1024;
	height = 1024;
	mapSize = width * height;
	quadSize = 16;
}
heightMap::~heightMap()
{
	
}

bool heightMap::loadRawFile(char* fileName)
{
	bool loadFromFile = true;
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
		if (result)
		{
			// Reading failed
			loadFromFile = false;
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
int heightMap::getHeight(int _x, int _y)
{
	// Force x and y to cap out at (mapSize - 1)
	int x = _x % mapSize;
	int y = _y % mapSize;

	// Check if empty
	if (!g_HeightMap)
	{
		// TODO: printf to console
		return 0;
	}

	// Treat the array like a 2D array (.raw format is a single array)
	return g_HeightMap[x + (y * mapSize)];	// Index into our height array and return the height
}

void heightMap::SetVertexColor(int x, int y)
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

	// Assign shade to the current vertex
	glColor3f(fColor, fColor, fColor);
}

void heightMap::renderHeightMap()
{
	if (!g_HeightMap)
	{
		// TODO: printf to console
		return;
	}

	glBegin(GL_QUADS);	// Render Quads
	int x, y, z;		// For readability/visuality (Where the map is in the dimension)

	for (int _w = 0; _w < mapSize; _w += quadSize) //_width of map
	{
		for (int _h = 0; _h < mapSize; _h += quadSize) //_height of map
		{
			// Get (X, Y, Z) value for bottom left vertex
			x = _w;
			y = getHeight(_w, _h);
			z = _h;
			SetVertexColor(_w, _h);			// Set color value for current vertix
			glVertex3i(x, y, z);			// Sends vertex to rendering

			// Get (X, Y, Z) value for top left vertex
			x = _w;
			y = getHeight(_w, _h + quadSize);
			z = _h + quadSize;
			SetVertexColor(_w, _h);
			glVertex3i(x, y, z);

			// Get (X, Y, Z) value for top right vertex
			x = _w + quadSize;
			y = getHeight(_w + quadSize, _h + quadSize);
			z = _h + quadSize;
			SetVertexColor(_w, _h);
			glVertex3i(x, y, z);

			// Get (X, Y, Z) value for bottom right vertex
			x = _w + quadSize;
			y = getHeight(_w + quadSize, _h);
			z = _h;
			SetVertexColor(_w, _h);
			glVertex3i(x, y, z);
		}
	}
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Reset the color
}