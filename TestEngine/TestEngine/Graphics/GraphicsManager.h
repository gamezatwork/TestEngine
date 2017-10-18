#pragma once
#include "..\Core\IManager.h"

namespace Graphics {

	class GraphicsManager : public Core::IManager<GraphicsManager>
	{
	public:


	private:

		GraphicsManager(){}

		// The init function which is called at the start (returns false if it failed)
		virtual bool _Init();

		// The update function which is called every loop (OVERRIDE THIS ONE)
		virtual void _Update();

		// The shut down function which is called at the end (OVERRIDE THIS ONE)
		virtual void _ShutDown();

		friend class Core::IManager<GraphicsManager>;

	};

}