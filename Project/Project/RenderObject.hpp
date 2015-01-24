#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include <GL/glew.h>

class renderObject
{
public:
	renderObject();
	virtual ~renderObject();

	void render();

	void genBuffer();
	void fillBuffer();

private:

	bool generated;

	float ry = 0;

	GLuint vBuffer;
	GLuint vArray;
	GLuint indexBuffer;

	GLuint vs, fs;

	GLuint gShaderProgram;
};

#endif