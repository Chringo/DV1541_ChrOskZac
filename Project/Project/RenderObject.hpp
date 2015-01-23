#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include <GL/glew.h>

class renderObject
{
public:
	renderObject();
	virtual ~renderObject();

	void render();

private:
	GLuint vBuffer;
	GLuint vArray;

	GLuint gShaderProgram;
};

#endif