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

	bool isThreadRunning() const;

private:

	void renderThread();
	void render();

	GLFWwindow* window;

	std::thread* threadPointer;

	bool threadRunning;
};

#endif