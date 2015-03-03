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
	GLuint indexSize;

	struct objBuffer
	{
		GLfloat vertices[3];
		//GLfloat uvs[2];
		GLfloat vns[3];
	};
	struct myVec3
	{
		GLfloat pos[3];
		bool operator==(const myVec3 & right)
		{
			bool toReturn = false;
			if (
				this->pos[0] == right.pos[0] &&
				this->pos[1] == right.pos[1] &&
				this->pos[2] == right.pos[2]
				)
			{
				toReturn = true;
			}
			return toReturn;
		}
	};
	struct myVec2
	{
		GLfloat pos[2];
		bool operator==(const myVec2 & right)
		{
			bool toReturn = false;
			if (
				this->pos[0] == right.pos[0] &&
				this->pos[1] == right.pos[1] 
				)
			{
				toReturn = true;
			}
			return toReturn;
		}
	};
	struct mtlContainer
	{
		GLfloat Ns; //specular exponent
		myVec3 Ka;
		myVec3 Kd;
		myVec3 Ks;
		//d (Tr) Transparency (not going to be implemented)
		//illum (Illumination models) (not going to be implemented)
	};

	bool loadOBJ(std::string path, std::string & mtlFileName, std::vector < objBuffer > & out_objVec, std::vector <GLuint> & out_indexes);
	bool loadMTL(std::string path, mtlContainer& mtl);

	float ry = 0;

	// vArray should be moved out from this class
	// and probably bound to a shader within its own class

	GLuint vBuffer;
	GLuint vArray;
	GLuint indexBuffer;

	glm::mat4 modelMatrix;
};

#endif