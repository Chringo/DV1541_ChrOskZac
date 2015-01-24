#include <GL\glew.h>
#include "RenderWindow.hpp"

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
	obj.fillBuffer();
}

void renderWindow::renderThread()
{
	glfwMakeContextCurrent(window);
	// enable vsync
	glfwSwapInterval(1);

	// not sure why I need to make my object here
	obj.genBuffer();

	while (!glfwWindowShouldClose(window))
	{
		render();
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

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();*/
	/*glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

	glBegin(GL_TRIANGLES);
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(-0.6f, -0.4f, 0.f);
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.6f, -0.4f, 0.f);
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.6f, 0.f);
	glEnd();*/

	obj.render();

	glfwSwapBuffers(window);
}