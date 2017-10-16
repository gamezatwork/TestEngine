#pragma once
#include "..\Core\IManager.h"

namespace Input {

	class InputManager : public Core::IManager<InputManager>
	{
	public:

		// Returns the mouse position in the given parameters
		void GetMousePos(float& x, float& y) const;

	private:

		InputManager():_mousePosX(), _mousePosY(){}

		// The init function which is called at the start (returns false if it failed)
		virtual bool _Init();

		// The update function which is called every loop (OVERRIDE THIS ONE)
		virtual void _Update();

		// The shut down function which is called at the end (OVERRIDE THIS ONE)
		virtual void _ShutDown();


		// The mouse position for this frame
		float _mousePosX;
		float _mousePosY;

		friend class Core::IManager<InputManager>;

	};

}