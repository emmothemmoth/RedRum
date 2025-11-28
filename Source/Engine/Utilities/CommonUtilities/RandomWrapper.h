#pragma once
namespace CommonUtilities
{
	class RandomWrapper
	{
	public:
		RandomWrapper() = default;
		~RandomWrapper() = default;

		int GetRandomInt(const int& aFloor, const int& aRoof);
		int GetRandomIntNonZero(const int& aFloor, const int& aRoof);
		float GetRandomFloat(const float& aFloor, const float& aRoof);
		double GetRandomDouble(const double& aFloor, const double& aRoof);
		bool GetRandomBool();
	private:
	};

}