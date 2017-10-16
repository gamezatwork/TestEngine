#pragma once

namespace Core
{
	// The interface class that all managers will inherit from (singleton type) (using CRTP)
	template <typename ManagerType>
	class IManager
	{
	public:

		// Singleton gettor
		static ManagerType& Get()
		{
			static ManagerType instance;
			return instance;
		}

		// The init function which is called at the start (returns false if it failed)
		bool Init()
		{
			return _Init();
		}
		
		// The update function which is called every loop (REMEMBER TO CALL THIS AT THE END)
		void Update()
		{
			_Update();
		}

		// The shut down function which is called at the end (REMEMBER TO CALL THIS AT THE END)
		void ShutDown()
		{
			_ShutDown();
		}

	private:

		// The init function which is called at the start (returns false if it failed) (OVERRIDE THIS ONE)
		virtual bool _Init() = 0;

		// The update function which is called every loop (OVERRIDE THIS ONE)
		virtual void _Update() = 0;

		// The shut down function which is called at the end (OVERRIDE THIS ONE)
		virtual void _ShutDown() = 0;

	};


}
