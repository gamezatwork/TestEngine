#include <iostream>
#include <string>
#include <functional>
#include <vector>

#include "Reflection.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <imgui.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Called when the window is resized
void Callback_FrameBufferSize(GLFWwindow* window, int newWidth, int newHeight)
{
	// Set up the glviewport again
	glViewport(0, 0, newWidth, newHeight);
}

// Process the input
void ProcessInput(GLFWwindow* window)
{
	// If it is the escape key, close
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		// Close the window
		glfwSetWindowShouldClose(window, true);
	}
}

int main()
{
	// Set up glfw
	// Starts up the glfw library
	glfwInit();
	// Hint to what versions you want the glfw to be for opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "THIS IS A WINDOW", nullptr, nullptr);
	// If it failed, kill the program
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Then set the window as the current context
	glfwMakeContextCurrent(window);

	// Set up GLAD by settling the opengl function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to intialize GLAD!" << std::endl;
		return -1;
	}


	// Next, set the OpenGL viewport
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	// And set the callback for when the window is resized
	glfwSetFramebufferSizeCallback(window, Callback_FrameBufferSize);

	Reflection::Test::RunReflectionTest();


	// FINALLY WE DRAW
	while (!glfwWindowShouldClose(window))
	{
		// Process the input
		ProcessInput(window);

		// RENDER HERE

		// Set the background colour
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Clear!
		glClear(GL_COLOR_BUFFER_BIT);

		// swap the bufers
		glfwSwapBuffers(window);
		// Check for events
		glfwPollEvents();
	}

	// Ok, time to end
	glfwTerminate();

	return 0;
}