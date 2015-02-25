#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <string>

using std::string;

struct Light
{
	unsigned int id;
	float x, y, z;
	float r, g, b;
	float distance;
};

Light * readLights(std::string file, unsigned int &nrLights);

#endif