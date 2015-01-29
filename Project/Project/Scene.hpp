#ifndef SCENE_HPP
#define SCENE_HPP

#include "RenderObject.hpp"
#include <vector>

#include "Camera.hpp"

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


using std::vector;

class scene
{
public:
	scene();
	~scene();

	// render and update function
	void renderScene();
	void updateScene();


	void requestBuffer();

	camera &getCamera();

private:


	camera cam;

	void frameUpdate();
	void generateShader();

	GLuint shaderProgram;

	// when we have multiple objects render them all using loops
	vector<renderObject> objects;
	vector<renderObject> newRegisterObjects;

	// view projection matrices
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// our temp object
	renderObject obj;
};

#endif