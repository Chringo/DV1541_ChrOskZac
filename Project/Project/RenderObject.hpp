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
	struct myVec3
	{
		GLfloat x, y, z;
	};
	struct myVec2
	{
		GLfloat x, y;
	};
	struct IndexContainer
	{
		GLushort v1, v2, v3;
		GLushort vt1, vt2, vt3; 
		GLushort vn1, vn2, vn3;
	};


	bool loadOBJ(const char * path, std::vector < myVec3 > & out_vertices, std::vector < myVec2 > & out_uvs, std::vector < myVec3 > & out_normals, std::vector <IndexContainer> & out_indexes);

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