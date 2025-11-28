#include "CommonUtilities.pch.h"
#include "RandomWrapper.h"
#include <random>

namespace CommonUtilities
{

	int RandomWrapper::GetRandomInt(const int& aFloor, const int& aRoof)
	{
		std::random_device seed;
		std::mt19937 rndEngine(seed());
		std::uniform_int_distribution<int> distributionInt(aFloor, aRoof);
		int randomInt = distributionInt(rndEngine);
		return randomInt;
	}

	int RandomWrapper::GetRandomIntNonZero(const int& aFloor, const int& aRoof)
	{
		std::random_device seed;
		std::mt19937 rndEngine(seed());
		std::uniform_int_distribution<int> distributionInt(aFloor, aRoof);
		int randomInt = distributionInt(rndEngine);
		if (randomInt == 0)
		{
			randomInt = 1;
		}
		return randomInt;
	}

	float RandomWrapper::GetRandomFloat(const float& aFloor, const float& aRoof)
	{
		std::random_device seed;
		std::mt19937 rndEngine(seed());
		std::uniform_int_distribution<int> distributionInt(static_cast<int>(aFloor), static_cast<int>(aRoof) * 100);
		float randomFloat = (distributionInt(rndEngine) / static_cast<float>(100));
		return randomFloat;
	}
	double RandomWrapper::GetRandomDouble(const double& aFloor, const double& aRoof)
	{
		std::random_device seed;
		std::mt19937 rndEngine(seed());
		std::uniform_int_distribution<int> distributionInt(static_cast<int>(aFloor), static_cast<int>(aRoof) * 1000);
		double randomDouble = (distributionInt(rndEngine) / static_cast<double>(1000));
		return randomDouble;
	}
	bool RandomWrapper::GetRandomBool()
	{
		std::random_device seed;
		std::mt19937 rndEngine(seed());
		std::uniform_int_distribution<int> distributionInt(0, 1);
		int randomInt = distributionInt(rndEngine);
		if (randomInt == 1)
		{
			return true;
		}
		return false;
	}
}