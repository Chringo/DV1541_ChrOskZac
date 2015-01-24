#ifndef SCENE_HPP
#define SCENE_HPP

#include "RenderObject.hpp"

class scene
{
public:
	scene();
	~scene();

	void renderScene();
	void updateScene();

	void requestBuffer();

private:
	renderObject obj;
};

#endif