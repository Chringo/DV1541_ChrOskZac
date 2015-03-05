#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <string>

using std::string;

struct Light
{
	float x, y, z, w;
	float r, g, b, distance;
	//float distance;
};

Light * readLights(std::string file, unsigned int &nrLights);

#endif