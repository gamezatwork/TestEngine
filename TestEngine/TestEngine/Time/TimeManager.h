#pragma once

#include <chrono>

#include "../Core/IManager.h"

namespace Time {

	// Will help you find out the delta time and stuff
	class TimeManager : public Core::IManager<TimeManager>
	{
	public:

		// Returns the actual dt of each frame
		double GetDeltaTime() const { return _dt; }
		// Returns the FPS (0 if _dt is 0)
		double GetFPS() const { return (_dt == 0.0) ? 0.0 : (1.0 / _dt); }

	private:

		TimeManager():_dt(1.0/60.0), _startClockTime(std::chrono::system_clock::now()), _prevClockTime(std::chrono::system_clock::now()){}

		// The init function which is called at the start (returns false if it failed)
		virtual bool _Init();

		// The update function which is called every loop (OVERRIDE THIS ONE)
		virtual void _Update();

		// The shut down function which is called at the end (OVERRIDE THIS ONE)
		virtual void _ShutDown();

		// The dt for this frame (will be 0 at the start)
		double _dt;

		// Starting point
		std::chrono::time_point<std::chrono::system_clock> _startClockTime;
		// Prev point
		std::chrono::time_point<std::chrono::system_clock> _prevClockTime;

		friend class Core::IManager<TimeManager>;

	};

}
