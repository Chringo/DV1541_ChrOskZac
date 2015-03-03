#include "RenderObject.hpp"

#define GLM_FORCE_RADIANS

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

renderObject::renderObject()
{
	modelMatrix = glm::mat4(1.0);

	generated = false;

	width = 1024;
	height = 1024;
	mapSize = width * height;
	quadSize = 1;
	g_HeightMap = new unsigned char[mapSize];

	const int i = 1024;
	vMap = new vertexMap[i*i];
	rgbColor = 1;
}

void renderObject::genBuffer(GLuint shader)
{
	glGenBuffers(1, &dataMap);
	glBindBuffer(GL_ARRAY_BUFFER, dataMap);

	int x, y, z;		// For readability/visuality (Where the map is in the dimension)
	int vCount = 0;
	for (int _w = 0; _w < width; _w += quadSize) //_width of map
	{
		for (int _h = 0; _h < height; _h += quadSize) //_height of map
		{
			// Get (X, Y, Z) value for bottom left vertex
			x = _w;
			y = getHeight(_w, _h);
			z = _h;
			vMap[vCount] = vertexMap{ x, y, z, rgbColor, rgbColor, rgbColor };
			vCount++;
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexMap) * vCount, &vMap[0], GL_DYNAMIC_DRAW); // GL_STATIC_DRAW replaced

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	struct indexArray
	{
		GLuint v1, v2, v3;
	};

	std::vector<indexArray> indexHolder;
	for (int _w = 0; _w < width - 1; _w += quadSize) //_width of map
	{
		for (int _h = 0; _h < (height - 1); _h += quadSize) //_height of map
		{
			indexArray index;
			index.v1 = _h + (1024 * _w);
			index.v2 = (_h + 1) + (1024 * (_w + 1));
			index.v3 = (_h + 1) + (1024 * _w);

			indexArray index2;
			index2.v1 = index.v1;
			index2.v2 = _h + (1024 * (_w + 1));
			index2.v3 = index.v2;

			indexHolder.push_back(index);
			indexHolder.push_back(index2);
		}
	}
	nrIndex = indexHolder.size();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexArray) * indexHolder.size(), &indexHolder[0], GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vArray);
	glBindVertexArray(vArray);

	glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
	glEnableVertexAttribArray(1);


	/// this should be moved out from this class
	/// as it is bound to shader, and can be used across multiple objects
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexMap), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertexMap), BUFFER_OFFSET(sizeof(float)* 3));
	/// 

	generated = true;
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

renderObject::~renderObject()
{
	delete g_HeightMap;
}

void renderObject::render()
{
	
	glBindBuffer(GL_ARRAY_BUFFER, dataMap);
	glBindVertexArray(vArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawElements(GL_TRIANGLES, 12 * nrIndex, GL_UNSIGNED_INT, (void*)0);
/*	glDrawArrays(GL_TRIANGLE_STRIP, 0, 256 * 256);*/

}

bool renderObject::loadRawFile(char* fileName)
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

int renderObject::getHeight(int _x, int _y)
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