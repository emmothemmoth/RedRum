#include "Audioengine.pch.h"

#include "JuceLibraryCode/AppConfig.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h> 

#include "AudioEngine.h"

#include <unordered_map>
#include <optional>

struct AudioFile
{
    juce::AudioSampleBuffer Buffer;
    double SampleRate = 0;
};

struct AudioSource
{
    void Process(juce::AudioBuffer<float>& outputBuffer)
    {
        if (!IsPlaying || CurrentSound == nullptr) return;

        auto& soundBuffer = CurrentSound->Buffer;
        int samplesToCopy = outputBuffer.getNumSamples();
        int soundLength = soundBuffer.getNumSamples();

        for (int chan = 0; chan < outputBuffer.getNumChannels(); ++chan)
        {
            // Basic wrap-around logic for looping or stopping
            int remaining = soundLength - ReadIndex;
            int toCopy = std::min(samplesToCopy, remaining);

            outputBuffer.copyFrom(chan, 0, soundBuffer, chan % soundBuffer.getNumChannels(), ReadIndex, toCopy);

            // If the buffer is longer than the remaining sound, handle the end
            if (toCopy < samplesToCopy && Loop)
            {
                outputBuffer.copyFrom(chan, toCopy, soundBuffer, chan % soundBuffer.getNumChannels(), 0, samplesToCopy - toCopy);
            }
        }

        ReadIndex += samplesToCopy;
        if (ReadIndex >= soundLength)
        {
            if (Loop) ReadIndex %= soundLength;
            else { IsPlaying = false; ReadIndex = 0; }
        }
    }
    CommonUtilities::Matrix4x4f Transform;
    AudioFile* CurrentSound = nullptr;
    int ReadIndex = 0;
    bool IsPlaying = false;
    bool Loop = false;
};

struct AudioListener
{

};

struct AudioEngine::Impl : public juce::AudioIODeviceCallback
{
	void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
		int numInputChannels,
		float* const* outputChannelData,
		int numOutputChannels,
		int numSamples,
		const juce::AudioIODeviceCallbackContext& context) override;

	void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
	void audioDeviceStopped() override;

	void InitListener(const CU::Matrix4x4f& aMatrix);
	std::optional<AudioHandle> RegisterSoundSource(const std::filesystem::path& aFilePath);
	void UnregisterSoundSource();

	void UpdateSoundSource(const AudioHandle, const CU::Matrix4x4f& aMatrix);

	juce::AudioDeviceManager DeviceManager;
	std::unordered_map<AudioHandle, AudioSource> AudioSources;
	AudioHandle HandleCounter;
};

AudioEngine::AudioEngine()
{
}

AudioEngine::~AudioEngine()
{
	myImpl->DeviceManager.removeAudioCallback(myImpl.get());
}

void AudioEngine::Initialize()
{
    if (!myImpl)
    {
        myImpl = std::make_unique<Impl>();
        myIsInitialized = true;
        myImpl->DeviceManager.initialiseWithDefaultDevices(0, 2);
        myImpl->DeviceManager.addAudioCallback(myImpl.get());
    }
}

void AudioEngine::InitListener(const CommonUtilities::Matrix4x4f& aMatrix)
{
    aMatrix;
}

std::optional<AudioHandle> AudioEngine::RegisterSoundSource(const std::filesystem::path& aFilePath)
{
	bool success = false;
	//if file could be loaded'
    aFilePath;

	if (success) return myImpl->HandleCounter++;
	return {};
}

void AudioEngine::UnregisterSoundSource()
{
}

void AudioEngine::UpdateSoundSource(const AudioHandle, const CU::Matrix4x4f& aMatrix)
{
    aMatrix;
}

void AudioEngine::Impl::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels, float* const* outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext& context)
{
    context;
    numInputChannels;
    inputChannelData;
    // 1. Create the AudioBlock. 
        // This class is a "view" and does not allocate memory.
        // It accepts the float* const* pointers directly.
    juce::dsp::AudioBlock<float> block(const_cast<float**>(outputChannelData),
        numOutputChannels,
        numSamples);

    // 2. Use the block like a buffer
    block.clear();

    // 3. If you need to pass it to a function that REQUIRES an AudioBuffer,
    // you can process it here or use Option 2 below.
}

void AudioEngine::Impl::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    device;
}

void AudioEngine::Impl::audioDeviceStopped()
{
}
