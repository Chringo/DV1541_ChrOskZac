#ifndef SCENE_HPP
#define SCENE_HPP

#include "RenderObject.hpp"
#include <vector>
#include "GBuffer.hpp"

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>


using std::vector;

class scene
{
public:
	scene();
	~scene() {};

	// render and update function
	void renderScene();
	void updateScene();

	bool requestBuffer(int width, int height);
	void frameUpdate();

	camera &getCamera();

	void screenChanged();
	void queueReloadShader();

private:

	bool updateGBuffer;
	bool reloadShader;
	
	GBuffer gBuffer;

	camera cam;

	void generateShader();

	GLuint fboShader;

	// view projection matrices
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	glm::mat4 shadowViewMat;

	// our object
	renderObject obj;
};

#endif