#include "WindowManager.h"

#include <string>
#include <iostream>

namespace Core {

	// The init function which is called at the start (returns false if it failed)
	bool WindowManager::_Init()
	{
		// Starts up the glfw library
		glfwInit();
		// Hint to what versions you want the glfw to be for opengl
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Create the main window
		Window window = CreateNewWindow(DEFAULT_WINWIDTH, DEFAULT_WINHEIGHT, "Main");
		if (window.winPtr == nullptr)
		{
			std::cout << "Failed to create GLFW window!" << std::endl;
			glfwTerminate();
			return false;
		}


		/*
		GLFWwindow* window = glfwCreateWindow(DEFAULT_WINWIDTH, DEFAULT_WINHEIGHT, "Main", nullptr, nullptr);
		// If it failed, kill the program
		if (window == nullptr)
		{
			std::cout << "Failed to create GLFW window!" << std::endl;
			glfwTerminate();
			return -1;
		}
		*/
		// Then set the window as the current context
		glfwMakeContextCurrent(window.winPtr);

		// Set up GLAD by settling the opengl function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to intialize GLAD!" << std::endl;
			return false;
		}


		return true;
	}

	// The update function which is called every loop (OVERRIDE THIS ONE)
	void WindowManager::_Update()
	{

	}

	// The shut down function which is called at the end (OVERRIDE THIS ONE)
	void WindowManager::_ShutDown()
	{

	}

	// Call to create a window
	Window WindowManager::CreateNewWindow(int width, int height, std::string winName)
	{
		// Create the main window
		GLFWwindow* window = glfwCreateWindow(width, height, winName.c_str(), nullptr, nullptr);
		// If it failed, kill the program
		if (window == nullptr)
		{
			std::cout << "Failed to create GLFW window " << winName << std::endl;
			return Window();
		}

		Window newWindow;
		newWindow.index = _windows.size();
		newWindow.width = width;
		newWindow.height = height;
		newWindow.winPtr = window;

		_windows.push_back(newWindow);

		return newWindow;

	}
	// Call to destroy a window (watch out because this screws up the vector)
	bool WindowManager::DestroyWindow(int index)
	{

	}

}