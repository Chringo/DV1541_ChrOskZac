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

	mapWidth = 1024;
	mapHeight = 1024;
	mapSize = mapWidth * mapHeight;
	quadSize = 2;
	g_HeightMap = new unsigned char[mapSize];

	gridWidth = mapWidth / quadSize;
	gridHeight = mapHeight / quadSize;

	vertices = new VertexPosition[gridHeight * gridWidth];	// Allocate memory for the individual vertices of the terrain
	rgbColor = 1.0f;

	qLevels = 4;
}
renderObject::~renderObject()
{
	delete g_HeightMap;
	delete vertices;

	releaseQuadTree(quadTree);
}

void renderObject::genBuffer(GLuint shader)
{
	glGenBuffers(1, &VBOHeightMap);
	glBindBuffer(GL_ARRAY_BUFFER, VBOHeightMap);

	int vIndex = 0;
	for (int _w = 0; _w < mapWidth; _w += quadSize)
	{
		for (int _h = 0; _h < mapHeight; _h += quadSize)
		{
			rgbColor = setVertexColor(_w, _h);
			vertices[vIndex++] = 
				VertexPosition{ 
				static_cast<GLfloat>(_w), static_cast<GLfloat>(getHeight(_w, _h)), static_cast<GLfloat>(_h),
				rgbColor, rgbColor, rgbColor 
			};
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPosition)* vIndex, &vertices[0], GL_STATIC_DRAW);

	quadTree = createQuadTree(qLevels, 0.0f, 0.0f, static_cast<GLfloat>(mapWidth), static_cast<GLfloat>(mapHeight));

	glGenVertexArrays(1, &VAOHeightMap);
	glBindVertexArray(VAOHeightMap);

	glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), BUFFER_OFFSET(sizeof(float) * 3));

	generated = true;
}

void renderObject::update()
{
	checkQuadTree(quadTree);
}

void renderObject::checkQuadTree(QuadTree* qt)
{
	float size = qt->size;
	glm::vec4 posOffset[] = {
		glm::vec4(size, size, size, 0.0f),
		glm::vec4(size, size, -size, 0.0f),
		glm::vec4(-size, size, size, 0.0f),
		glm::vec4(-size, size, -size, 0.0f),
	};


	bool inside = false;
	for (int o = 0; o < 4 && !inside; o++)
	{
		glm::vec4 pos = view * (glm::vec4(qt->x, qt->z, qt->y, 1.0f) + posOffset[o]);
		float dist;
		float rad = 1.0f;
		bool inFrustum = true;
		for (int i = 4; i >= 0 && inFrustum; i--)
		{
			if (i == 2 || i == 3) continue;
			dist = glm::dot(frustumPlanes[i], pos);
			inFrustum = (-rad <= dist);
		}

		dist = glm::dot(frustumPlanes[5], pos);

		if (dist < -1)
		{
			inFrustum = false;
		}

		inside = inFrustum;
	}

	
	if (inside)
	{
		if (qt->botLeft)
		{
			checkQuadTree(qt->botLeft);
			checkQuadTree(qt->botRight);
			checkQuadTree(qt->topLeft);
			checkQuadTree(qt->topRight);
		}
	}
	qt->visible = inside;
}

const GLfloat * renderObject::getModelMatrix() const
{
	return &modelMatrix[0][0];
}


void renderObject::render()
{

	renderCount = 0;
	glBindBuffer(GL_ARRAY_BUFFER, VBOHeightMap);
	glBindVertexArray(VAOHeightMap);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	// draw points 0-3 from the currently bound VAO with current in-use shader
	//glDrawElements(GL_TRIANGLES, 12 * nrIndex, GL_UNSIGNED_INT, (void*)0);
	renderQuadTree(quadTree);

	if (renderCount)
	{
		renderCount = renderCount;
	}
}

void renderObject::renderQuadTree(QuadTree* qt)
{
	if (qt->visible)
	{
		renderCount++;
		if (qt->botLeft)
		{
			renderQuadTree(qt->botLeft);
			renderQuadTree(qt->botRight);
			renderQuadTree(qt->topLeft);
			renderQuadTree(qt->topRight);
		}
		else
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qt->q_IndexBuffer);
			// draw points 0-3 from the currently bound VAO with current in-use shader
			glDrawElements(GL_TRIANGLES, 12 * qt->nrIndex, GL_UNSIGNED_INT, (void*)0);
		}
	}
}
void renderObject::releaseQuadTree(QuadTree* qt)
{
	if (qt->botLeft)
	{
		releaseQuadTree(qt->botLeft);
		releaseQuadTree(qt->botRight);
		releaseQuadTree(qt->topLeft);
		releaseQuadTree(qt->topRight);
	}
	delete qt;
}

QuadTree* renderObject::createQuadTree(int levels, GLfloat startX, GLfloat startY, GLfloat endX, GLfloat endY)
{
	QuadTree* root = new QuadTree();

	GLfloat x = ((endX - startX) * 0.5f) + startX;
	GLfloat y = ((endY - startY) * 0.5f) + startY;
	GLfloat size = abs((endX - startX) * 0.5f);

	root->x = x;
	root->y = y;
	root->size = size;

	root->q_IndexBuffer = 0;
	if (levels != 0)
	{

		QuadTree* topLeft = createQuadTree(levels - 1, x - size, y, x, y - size);
		QuadTree* topRight = createQuadTree(levels - 1, x, y - size, x + size, y);
		QuadTree* botLeft = createQuadTree(levels - 1, x - size, y, x, y + size);
		QuadTree* botRight = createQuadTree(levels - 1, x, y, x + size, y + size);

		root->topLeft = topLeft;
		root->topRight = topRight;
		root->botLeft = botLeft;
		root->botRight = botRight;
	}
	else
	{
		root->topLeft = nullptr;
		root->topRight = nullptr;
		root->botLeft = nullptr;
		root->botRight = nullptr;

		glGenBuffers(1, &root->q_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, root->q_IndexBuffer);

		struct IndexTriangle
		{
			GLuint v0, v1, v2;
		};

		std::vector<IndexTriangle> indexHolder;
		
		int i_size = static_cast<int>(size);
		int countSizeX = (i_size * 2) / quadSize;
		int countSizeY = (i_size * 2) / quadSize;

		int xOffset = static_cast<int>(abs(x - i_size)) / quadSize;
		int yOffset = static_cast<int>(abs(y - i_size)) / quadSize;


		// offset fix for last triangle row
		if (gridWidth - countSizeX == xOffset)
		{
			countSizeX--;
		}
		if (gridWidth - countSizeY == yOffset)
		{
			countSizeY--;
		}

		for (int _w = 0; _w < countSizeX; _w++)
		{
			for (int _h = 0; _h < countSizeY; _h++)
			{
				GLuint vertexIndex = ((_w + xOffset) * gridWidth) + (_h + yOffset);

				IndexTriangle top;
				top.v0 = vertexIndex;
				top.v1 = vertexIndex + gridWidth + 1;
				top.v2 = vertexIndex + 1;

				IndexTriangle bottom;
				bottom.v0 = top.v0;
				bottom.v1 = vertexIndex + gridWidth;
				bottom.v2 = top.v1;

				indexHolder.push_back(top);
				indexHolder.push_back(bottom);
			}
		}

		root->nrIndex = indexHolder.size();

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndexTriangle)* root->nrIndex, &indexHolder[0], GL_STATIC_DRAW);

	}
	return root;
}

void renderObject::createViewFrustum(glm::mat4 proj, glm::mat4 view, glm::vec2 screenSize)
{
	glm::vec2 tileScale = screenSize * (1.0f / screenSize);
	glm::vec2 tileBias = tileScale - glm::vec2(1.0f, 1.0f);

	glm::vec4 c1 = glm::vec4(-proj[0][0] * tileScale.x, 0.0f, tileBias.x, 0.0f);
	glm::vec4 c2 = glm::vec4(0.0f, -proj[1][1] * tileScale.y, tileBias.y, 0.0f);
	glm::vec4 c3 = glm::vec4(0.0f, 0.0f, -proj[2][2], 0.0f);
	glm::vec4 c4 = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);


	frustumPlanes[0] = c4 - c1;								// Right
	frustumPlanes[1] = c4 + c1;								// Left

	frustumPlanes[2] = c4 - c2;								// Bottom
	frustumPlanes[3] = c4 + c2;								// Top

	frustumPlanes[4] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) - c3;// glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);	// Near
	frustumPlanes[5] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f) + c3;// glm::vec4(0.0f, 0.0f, -1.0f, 1000.0f);	// Far

	for (int i = 0; i < 6; i++)
	{
		frustumPlanes[i] *= (1.0f / frustumPlanes[i].length());
	}

	this -> view = view;

}

/////////// --- HEIGHT MAP FUNCTIONS --- ///////////
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
	// Force x and y to cap at (mapSize - 1)
	int x = _x % mapWidth;
	int y = _y % mapHeight;

	// Check if empty
	if (!g_HeightMap)
	{
		// TODO: printf to console
		return 0;
	}

	// Treat the array like a 2D array (.raw format is a single array)
	return g_HeightMap[x + (y * mapHeight)];	// Index into our height array and return the height
}
GLfloat renderObject::setVertexColor(int x, int y)
{
	GLfloat color = -0.15f + (getHeight(x, y) / 256.0f);
	return color;
}
/////////// --- ==================== --- ///////////