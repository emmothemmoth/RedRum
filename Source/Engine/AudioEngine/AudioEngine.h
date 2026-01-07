#pragma once

namespace CommonUtilities
{
	class Vector3f;
}

class AudioEngine
{
public:
	AudioEngine();
	~AudioEngine();

	void ProcessBlock(float** someBuffers, int aNumChannels, int aNumSamples);

private:

};
