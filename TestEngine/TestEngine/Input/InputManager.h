#pragma once
#include "..\Core\CoreManager.h"

namespace Input {

	class InputManager : public Core::IManager<InputManager>
	{
	public:

	private:
		// The init function which is called at the start (returns false if it failed)
		virtual bool _Init();

		// The update function which is called every loop (OVERRIDE THIS ONE)
		virtual void _Update();

		// The shut down function which is called at the end (OVERRIDE THIS ONE)
		virtual void _ShutDown();
	};

}