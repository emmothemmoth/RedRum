#include "GameEngine.pch.h"
#include "MainSingleton.h"

MainSingleton& MainSingleton::Get()
{
	static MainSingleton myInstance;
	return myInstance;
}

AudioEngine& MainSingleton::GetAudioEngine()
{
	if (!myAudioEngine.IsInitialized())
	{
		myAudioEngine.Initialize();
	}
	return myAudioEngine;
}
