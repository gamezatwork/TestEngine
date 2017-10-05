#include "TimeManager.h"

namespace Time {

	// The init function which is called at the start (returns false if it failed)
	bool TimeManager::_Init()
	{
		// Init clock start point
		_startClockTime = std::chrono::system_clock::now();
		_prevClockTime = _startClockTime;

		return true;
	}

	// The update function which is called every loop (OVERRIDE THIS ONE)
	void TimeManager::_Update()
	{
		// Update the dt
		std::chrono::time_point<std::chrono::system_clock> currPt = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedTime = currPt - _prevClockTime;
		_prevClockTime = currPt;
		_dt = elapsedTime.count();
	}

	// The shut down function which is called at the end (OVERRIDE THIS ONE)
	void TimeManager::_ShutDown()
	{

	}

}