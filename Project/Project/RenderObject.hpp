#ifndef RENDEROBJECT_HPP
#define RENDEROBJECT_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

// All meshes are supposed to be in the Meshes folder so add this to all the paths
#define MESH_FOLDER "Meshes/"

class renderObject
{
public:
	renderObject();
	virtual ~renderObject();

	void render(GLuint shader);

	void genBuffer(GLuint shader);
	void update();

	// ability to get the modelmatrix for objects, these are unique
	const GLfloat * getModelMatrix() const;

	struct myVec3
	{
		GLfloat pos[3];
	};
	struct myVec2
	{
		GLfloat pos[2];
	};
	struct mtlContainer
	{
		GLfloat Ns; //specular exponent
		myVec3 Ka;
		myVec3 Kd;
		myVec3 Ks;
		//d (Tr) Transparency (not going to be implemented)
		//illum (Illumination models) (not going to be implemented)
		std::string texturePath;
	};

	mtlContainer mtl;

private:
	GLuint indexSize;

	struct objBuffer
	{
		GLfloat vertices[3];
		GLfloat uvs[2];
		GLfloat vns[3];
	};

	bool loadOBJ(std::string path, std::string & mtlFileName, std::vector < objBuffer > & out_objVec, std::vector <GLuint> & out_indexes);
	
	// Only supports to load one (1) material
	bool loadMTL(std::string path, mtlContainer& mtl);
	bool loadTexture(std::string path, GLuint &ID);

	float ry = 0;

	GLuint vBuffer;
	GLuint vArray;
	GLuint indexBuffer;

	GLuint textureID;

	glm::mat4 modelMatrix;
};

#endif