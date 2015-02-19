#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

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
	struct objBuffer
	{
		GLfloat vertices[3];
		//GLfloat uvs[2];
		GLfloat vns[3];
	};
	struct myVec3
	{
		GLfloat pos[3];
	};
	struct myVec2
	{
		GLfloat pos[2];
	};
	struct IndexContainer
	{
		GLuint v1, v2, v3;
		//GLushort vt1, vt2, vt3; 
		//GLuint vn1, vn2, vn3;
	};


	bool loadOBJ(const char * path, std::vector < objBuffer > & out_objVec, std::vector <IndexContainer> & out_indexes);

	GLuint indexSize;

	float ry = 0;

	// vArray should be moved out from this class
	// and probably bound to a shader within its own class

	GLuint vBuffer;
	GLuint vArray;
	GLuint indexBuffer;

	glm::mat4 modelMatrix;
};

#endif