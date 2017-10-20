#pragma once

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include "../Core/IManager.h"

#include <vector>

#define DEFAULT_WINWIDTH 800
#define DEFAULT_WINHEIGHT 600


namespace Core {

	// Just typedef the window so that it would be easier to fix in the future
	//typedef GLFWwindow Window;
	typedef GLFWwindow* WinPtr;

	struct Window {
		// The index of the window
		int index;
		// Width of the window
		int width;
		// Height of the window
		int height;
		// The pointer to the actual window
		WinPtr winPtr;

		Window():index(-1), width(0), height(0), winPtr(nullptr){}

	};



	// Handles the windows and stuff
	class WindowManager : public IManager<WindowManager>
	{
	public:

		// This gives you the pointer to the actual window
		WinPtr GetWindow();
		// And this gives you details about the window
		Window GetWindowInfo();


		// Call to create a window
		//Window CreateNewWindow(int width, int height, std::string winName);
		// Call to destroy a window (watch out because this screws up the vector)
		//bool DestroyWindow(int index);


	private:
		// The init function which is called at the start (returns false if it failed)
		virtual bool _Init();

		// The update function which is called every loop (OVERRIDE THIS ONE)
		virtual void _Update();

		// The shut down function which is called at the end (OVERRIDE THIS ONE)
		virtual void _ShutDown();

		// Call to create the window
		Window _CreateWindow(int width, int height, std::string winName);

		// All the windows that currently exist
		//std::vector<Window> _windows;
		Window _window;

		friend class IManager<WindowManager>;
	};

}