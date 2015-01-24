#include <GL\glew.h>
#include "RenderWindow.hpp"
#include <Windows.h>

renderWindow::renderWindow(GLFWwindow* window)
{
	this->window = window;
	threadRunning = false;
}

renderWindow::~renderWindow()
{

}

void renderWindow::createThread()
{
	threadPointer = new std::thread(&renderWindow::renderThread, this);
	if (threadPointer)
	{
		threadRunning = true;
	}
}

bool renderWindow::isThreadRunning() const
{
	return threadRunning;
}

void renderWindow::update()
{
	mainScene.updateScene();

	glfwSetWindowTitle(window, fpsCount.get().c_str());
}

void renderWindow::renderThread()
{
	glfwMakeContextCurrent(window);
	// enable vsync
	glfwSwapInterval(1);

	// not sure why I need to make my object here

	mainScene.requestBuffer();

	while (!glfwWindowShouldClose(window))
	{
		render();
		fpsCount.tick();
	}
	threadRunning = false;
}

void renderWindow::render()
{
	float ratio;
	int width, height;

	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	glViewport(0, 0, width, height);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);


	mainScene.renderScene();

	glfwSwapBuffers(window);
}