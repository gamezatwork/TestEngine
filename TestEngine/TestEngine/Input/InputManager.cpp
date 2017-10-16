#include <GLFW\glfw3.h>

#include "InputManager.h"

namespace Input {

	// Returns the mouse position in the given parameters
	void InputManager::GetMousePos(float& x, float& y) const
	{
		x = _mousePosX;
		y = _mousePosY;
	}



	// The init function which is called at the start (returns false if it failed)
	bool InputManager::_Init()
	{
		return true;
	}

	// The update function which is called every loop (OVERRIDE THIS ONE)
	void InputManager::_Update()
	{

	}

	// The shut down function which is called at the end (OVERRIDE THIS ONE)
	void InputManager::_ShutDown()
	{

	}

}