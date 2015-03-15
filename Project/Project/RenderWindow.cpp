#include <GL\glew.h>
#include "RenderWindow.hpp"
#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <Windows.h>
#include <iostream>

#ifdef _DEBUG
extern "C"
{
	void APIENTRY openglCallbackFunction(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		void* userParam)
	{

		std::cout << "---------------------opengl-callback-start------------" << std::endl;
		std::cout << "message: " << message << std::endl;
		std::cout << "type: ";
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			std::cout << "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cout << "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cout << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cout << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cout << "OTHER";
			break;
		}
		std::cout << std::endl;

		std::cout << "id: " << id << std::endl;
		std::cout << "severity: ";
		switch (severity){
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "HIGH";
			break;
		}
		std::cout << std::endl;
		std::cout << "---------------------opengl-callback-end--------------" << std::endl;
	}

}
#endif


renderWindow::renderWindow(GLFWwindow* window)
{
	this->window = window;
	threadRunning = false;
	speed = 1.0f;
}

renderWindow::~renderWindow()
{

}

void renderWindow::createThread()
{
	mainScene.getCamera().translation = glm::translate(mainScene.getCamera().translation, glm::vec3(0, -100, 0));
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

	glfwSetWindowTitle(window, fpsCount.get().c_str());

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		mainScene.getCamera().ry += (float)(xpos - oldx) / 50.0f;
		mainScene.getCamera().rx += (float)(ypos - oldy) / 50.0f;
		mainScene.getCamera().genRot();
		oldx = xpos;
		oldy = ypos;
	}
	else
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		oldx = xpos;
		oldy = ypos;
	}

	if (glfwGetKey(window, GLFW_KEY_F5))
	{
		mainScene.queueReloadShader();
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	if (width != windowX || height != windowY)
	{
		mainScene.screenChanged();
		windowX = width;
		windowY = height;
	}


	glm::vec3 up = mainScene.getCamera().rot * glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 forward = mainScene.getCamera().rot * glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 strafe = glm::cross(up, forward);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		mainScene.getCamera().translation = glm::translate(mainScene.getCamera().translation, forward*glm::vec3(speed, 0.0f, -speed));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		mainScene.getCamera().translation = glm::translate(mainScene.getCamera().translation, forward*glm::vec3(-speed, 0.0f, speed));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		mainScene.getCamera().translation = glm::translate(mainScene.getCamera().translation, strafe*glm::vec3(speed, 0.0f, -speed));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		mainScene.getCamera().translation = glm::translate(mainScene.getCamera().translation, strafe*glm::vec3(-speed, 0.0f, speed));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		mainScene.getCamera().translation = glm::translate(mainScene.getCamera().translation, glm::vec3(0.0f, -speed, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		mainScene.getCamera().translation = glm::translate(mainScene.getCamera().translation, glm::vec3(0.0f, speed, 0.0f));
	}

	glm::mat4 mat = mainScene.getCamera().translation;

	glm::vec3 camPos = glm::vec3(mat[3]);

	float x = -camPos[0];
	float z = -camPos[2];
	int y = 0;
	if (x > 0 && x < 1024 && z > 0 && z < 1024)
	{
		 y = mainScene.obj.getHeight(x, z);
		 mainScene.getCamera().translation[3] = glm::vec4(-x, -(y + 10), -z, 1.0);
	}
	
	int something = 0;

}

void renderWindow::renderThread()
{
	glfwMakeContextCurrent(window);


#ifdef _DEBUG
	if (glDebugMessageCallback){
		std::cout << "Register OpenGL debug callback " << std::endl;
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)openglCallbackFunction, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
	}
	else
	{
		std::cout << "glDebugMessageCallback not available" << std::endl;
	}
#endif


	// enable vsync
	glfwSwapInterval(1);

	// request buffers and init scene
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	mainScene.requestBuffer(width, height);

	while (!glfwWindowShouldClose(window))
	{
		render();
		fpsCount.tick();
		Sleep(1000 / 120);
	}
	threadRunning = false;
}


// render function
// would be possible to have multiple scenes to toggle between
void renderWindow::render()
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	mainScene.getCamera().height = (float)height;
	mainScene.getCamera().width = (float)width;
	mainScene.updateScene();
	mainScene.renderScene();

	glfwSwapBuffers(window);
}