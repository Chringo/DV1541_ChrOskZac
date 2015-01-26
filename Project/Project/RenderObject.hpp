#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

class renderObject
{
public:
	renderObject();
	virtual ~renderObject();

	void render();

	void genBuffer(GLuint shader);
	void update();

	// ability to get the modelmatrix for objects, these are unique
	const GLfloat * getModelMatrix() const;

private:

	void fillBuffer();

	bool generated;

	float ry = 0;

	// vArray should be moved out from this class
	// and probably bound to a shader within its own class

	GLuint vBuffer;
	GLuint vArray;
	GLuint indexBuffer;

	glm::mat4 modelMatrix;
};

#endif