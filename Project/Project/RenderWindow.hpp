#ifndef RENDERWINDOW_HPP
#define RENDERWINDOW_HPP

#include <glfw3.h>
#include <thread>

class renderWindow
{
	
public:
	renderWindow(GLFWwindow* window);
	virtual ~renderWindow();

	void createThread();

private:

	void renderThread();
	void render();

	GLFWwindow* window;

	std::thread* threadPointer;

};

#endif