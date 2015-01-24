#include "Scene.hpp"

scene::scene()
{

}

scene::~scene()
{

}

void scene::requestBuffer()
{
	obj.genBuffer();
}

void scene::renderScene()
{
	obj.render();
}

void scene::updateScene()
{
	obj.fillBuffer();
}