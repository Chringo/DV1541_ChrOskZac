#ifndef SCENE_HPP
#define SCENE_HPP

#include "RenderObject.hpp"
#include <vector>
#include "GBuffer.hpp"
#include "Camera.hpp"
#include "HeightMap.hpp"

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>


using std::vector;

class scene
{
public:
	scene();
	~scene();

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

	HeightMap* hMap;

	camera cam;

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