#include "CommonUtilities.pch.h"
#include "Timer.h"
#include <chrono>
#include "Vector.hpp"


namespace CommonUtilities
{

	Timer::Timer()
	{
		currentTime = std::chrono::high_resolution_clock::now();
		previousTime = std::chrono::high_resolution_clock::now();
		totalTime = 0.0;
		deltaTime = 0.0f;
	}

	static Timer localInstance;
	Timer& Timer::Get()
	{
		return localInstance;
	}
	

	void Timer::Update()
	{
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - previousTime).count();
		totalTime += static_cast<double> (deltaTime);
		previousTime = currentTime;
	}

	float Timer::GetDeltaTime() const
	{
		return deltaTime;
	}

	double Timer::GetTotalTime() const
	{
		return totalTime;
	}

	
}

