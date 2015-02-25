#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "RenderWindow.hpp"
#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <Windows.h>


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
	GLFWwindow* window = nullptr;

	glfwSetErrorCallback(error_callback);

	// glfw library init
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	
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

	/// this is needed for glewinit
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	else
	{
		fprintf(stderr, "glew initialized\n");
	}
	glfwMakeContextCurrent(NULL);
	///


	renderWindow rWindow(window);
	rWindow.createThread();
	// while window is open
	while (rWindow.isThreadRunning())
	{
		// check for events
		rWindow.update();
		glfwPollEvents();
		Sleep(1000/60);
	}

	// destroy the window
	glfwDestroyWindow(window);

	// free resources the library have used
	glfwTerminate();
	return 0;
}