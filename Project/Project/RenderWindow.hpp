#ifndef RENDERWINDOW_HPP
#define RENDERWINDOW_HPP

#include <GLFW/glfw3.h>
#include <thread>
#include "Scene.hpp"
#include "FPSCounter.hpp"

class renderWindow
{
	
public:
	renderWindow(GLFWwindow* window);
	virtual ~renderWindow();

	void createThread();

	// update function
	void update();

	// check if the thread is running
	bool isThreadRunning() const;

private:

	// render thread and render function
	void renderThread();
	void render();

	GLFWwindow* window;

	std::thread* threadPointer;

	bool threadRunning;

	// scene class
	scene mainScene;

	// fps counter class
	fpsCounter fpsCount;
};

#endif