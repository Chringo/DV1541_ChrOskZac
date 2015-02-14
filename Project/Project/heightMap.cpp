#include "heightMap.hpp"


heightMap::heightMap()
{
	width = 1024;
	height = 1024;
}
heightMap::~heightMap()
{

}

bool heightMap::loadDataMap()
{
	g_HeightMap[width * height];
	return true;
}