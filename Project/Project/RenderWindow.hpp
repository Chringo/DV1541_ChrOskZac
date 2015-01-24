#ifndef RENDERWINDOW_HPP
#define RENDERWINDOW_HPP

#include <GLFW/glfw3.h>
#include <thread>
#include "RenderObject.hpp"

class renderWindow
{
	
public:
	renderWindow(GLFWwindow* window);
	virtual ~renderWindow();

	void createThread();
	void update();

	bool isThreadRunning() const;

private:

	void renderThread();
	void render();

	GLFWwindow* window;

	std::thread* threadPointer;

	bool threadRunning;

	renderObject obj;
};

#endif