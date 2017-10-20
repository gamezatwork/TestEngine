#include "WindowManager.h"

#include <string>
#include <iostream>

namespace Core {

	// This gives you the pointer to the actual window
	WinPtr WindowManager::GetWindow()
	{
		return _window.winPtr;
	}
	// And this gives you details about the window
	Window WindowManager::GetWindowInfo()
	{
		return _window;
	}

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

		Window window = _CreateWindow(DEFAULT_WINWIDTH, DEFAULT_WINHEIGHT, "Main");
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
		// Ok, time to end
		glfwTerminate();
	}

	
	// Call to create a window
	Window WindowManager::_CreateWindow(int width, int height, std::string winName)
	{
		// Create the main window
		GLFWwindow* window = glfwCreateWindow(width, height, winName.c_str(), nullptr, nullptr);
		// If it failed, kill the program
		if (window == nullptr)
		{
			std::cout << "Failed to create GLFW window " << winName << std::endl;
			return Window();
		}

		//Window newWindow;
		_window.index = 0;
		_window.width = width;
		_window.height = height;
		_window.winPtr = window;

		//_windows.push_back(newWindow);

		return _window;

	}
	/*
	// Call to destroy a window (watch out because this screws up the vector)
	bool WindowManager::DestroyWindow(int index)
	{
		return true;
	}

	*/
}