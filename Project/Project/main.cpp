#include <glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "RenderWindow.hpp"



// error callback function
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

// window key callback function
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// main crap
int main()
{

	// window pointer
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	// glfw library init
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	// create a window with 400 400 in size
	window = glfwCreateWindow(400, 400, "Title", NULL, NULL);

	// if we failed creating a window
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// when a key is pressed can this function
	glfwSetKeyCallback(window, key_callback);

	renderWindow rWindow(window);
	rWindow.createThread();
	// while window is open
	while (!glfwWindowShouldClose(window))
	{
		// check for events
		glfwPollEvents();
	}

	// destroy the window
	glfwDestroyWindow(window);

	// free resources the library have used
	glfwTerminate();
	return 0;
}