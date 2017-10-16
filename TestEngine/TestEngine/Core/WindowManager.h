#pragma once

#include <GLFW\glfw3.h>

#include "../Core/IManager.h"

namespace Core {

	// Just typedef the window so that it would be easier to fix in the future
	typedef GLFWwindow Window;

	// Handles the windows and stuff
	class WindowManager : public IManager<WindowManager>
	{
	public:

		Window* GetWindow();

	private:
		// The init function which is called at the start (returns false if it failed)
		virtual bool _Init();

		// The update function which is called every loop (OVERRIDE THIS ONE)
		virtual void _Update();

		// The shut down function which is called at the end (OVERRIDE THIS ONE)
		virtual void _ShutDown();

		friend class IManager<WindowManager>;
	};

}