#include "GameEngine.pch.h"
#include "MainSingleton.h"

MainSingleton& MainSingleton::Get()
{
	static MainSingleton myInstance;
	return myInstance;
}
