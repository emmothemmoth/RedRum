#pragma once
#include <chrono>
namespace CommonUtilities
{
	class Timer
	{
	public:
		Timer();
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;
		static Timer& Get();

		void Update();

		float GetDeltaTime() const;
		double GetTotalTime() const;
	private:
		std::chrono::high_resolution_clock::time_point previousTime;
		std::chrono::high_resolution_clock::time_point currentTime;
		float deltaTime;
		double totalTime;
	};
}
namespace CU = CommonUtilities;


