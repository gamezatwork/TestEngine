#include "CoreManager.h"

namespace Core
{
	// Singleton gettor
	/*
	template <typename ManagerType>
	static ManagerType& IManager<ManagerType>::Get() {
		static ManagerType instance;
		return instance;
	}
	*/

	// The init function which is called at the start (returns false if it failed) (REMEMBER TO CALL THIS AT THE START)
	template <typename ManagerType>
	bool IManager<ManagerType>::Init()
	{
		return _Init();
	}

	// The update function which is called every loop (REMEMBER TO CALL THIS AT THE END)
	template <typename ManagerType>
	void IManager<ManagerType>::Update()
	{
		_Update();
	}

	// The shut down function which is called at the end (REMEMBER TO CALL THIS AT THE END)
	template <typename ManagerType>
	void IManager<ManagerType>::ShutDown()
	{
		_ShutDown();
	}

}