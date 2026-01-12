#include "Audioengine.pch.h"

#include "JuceLibraryCode/AppConfig.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
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
    // We need a move constructor because unique_ptr can't be copied
    AudioSource() : Interpolator(std::make_unique<juce::LagrangeInterpolator>()) {}
    AudioSource(AudioSource&& other) noexcept = default;
    AudioSource& operator=(AudioSource&& other) noexcept = default;

    // Delete the copy constructor and assignment to satisfy the compiler
    AudioSource(const AudioSource&) = delete;
    AudioSource& operator=(const AudioSource&) = delete;
    void Process(juce::AudioBuffer<float>& outputBuffer)
    {
        if (!IsPlaying || CurrentSound == nullptr) return;

        auto& soundBuffer = CurrentSound->Buffer;
        const int numChannels = outputBuffer.getNumChannels();
        const int numSamples = outputBuffer.getNumSamples();
        const int soundLength = soundBuffer.getNumSamples();

        // Check if we have enough samples left to even try
        if (ReadIndex >= soundLength)
        {
            if (Loop) ReadIndex = 0;
            else { IsPlaying = false; return; }
        }

        for (int chan = 0; chan < numChannels; ++chan)
        {
            int sourceChan = chan % soundBuffer.getNumChannels();

            // Get pointers for this specific channel
            const float* sourcePtr = soundBuffer.getReadPointer(sourceChan, ReadIndex);
            float* destPtr = outputBuffer.getWritePointer(chan);

            // JUCE LagrangeInterpolator::process
            Interpolator->process(Ratio, sourcePtr, destPtr, numSamples);
        }

        // Advance the read index based on how many input samples were consumed
        // numSamples (output) * Ratio (in/out) = input samples consumed
        ReadIndex += static_cast<int>(numSamples * Ratio);

        if (ReadIndex >= soundLength)
        {
            if (Loop) ReadIndex %= soundLength;
            else { IsPlaying = false; ReadIndex = 0; }
        }
    }
    CommonUtilities::Matrix4x4f Transform;
    AudioFile* CurrentSound = nullptr;
    std::unique_ptr<juce::LagrangeInterpolator> Interpolator;
    double Ratio = 1.0;
    int ReadIndex = 0;
    bool IsPlaying = false;
    bool Loop = false;
};

struct AudioCommand
{
    AudioCommand() {};
    AudioCommand(AudioCommand&&) noexcept = default;
    AudioCommand& operator=(AudioCommand&&) noexcept = default;

    enum { Play, Stop, UpdateTransform, AddSource } Type;
    AudioHandle Handle;
    std::unique_ptr<AudioSource> SourceData;
    // You can add Matrix4x4 here for 3D updates later
};

struct AudioListener
{

};

struct AudioEngine::Impl : public juce::AudioIODeviceCallback
{
    Impl();
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

    void PushCommand(AudioCommand aCommand);

    static constexpr int QueueSize = 128;
    juce::AbstractFifo CommandFifo{ QueueSize };
    AudioCommand CommandBuffer[QueueSize];

    std::vector<AudioSource> ActiveVoices;

    juce::AudioFormatManager FormatManager;
    std::unordered_map<std::string, std::shared_ptr<AudioFile>> LoadedFiles;

	juce::AudioDeviceManager DeviceManager;
	std::unordered_map<AudioHandle, std::unique_ptr<AudioSource>> AudioSources;
	AudioHandle HandleCounter = 0;
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
    // 1. Load the file (Heavy Work - Main Thread)
    juce::File file(aFilePath.string());
    std::unique_ptr<juce::AudioFormatReader> reader(myImpl->FormatManager.createReaderFor(file));

    if (reader == nullptr) return std::nullopt;

    auto newFile = std::make_shared<AudioFile>();
    newFile->SampleRate = reader->sampleRate;
    newFile->Buffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
    reader->read(&newFile->Buffer, 0, (int)reader->lengthInSamples, 0, true, true);

    // 2. Prepare the Source
    AudioHandle handle = myImpl->HandleCounter++;
    auto newSource = std::make_unique<AudioSource>();
    auto* device = myImpl->DeviceManager.getCurrentAudioDevice();
    double hardwareSampleRate = device ? device->getCurrentSampleRate() : 44100.0;
    newSource->Ratio = newFile->SampleRate / hardwareSampleRate;
    newSource->CurrentSound = newFile.get();
    newSource->IsPlaying = false;

    // 3. Keep the data alive in our "Library"
    myImpl->LoadedFiles[aFilePath.string()] = newFile;

    // 4. Send the command to the Audio Thread
    AudioCommand cmd;
    cmd.Type = AudioCommand::AddSource;
    cmd.Handle = handle;
    cmd.SourceData = std::move(newSource);
    myImpl->PushCommand(std::move(cmd));

    return handle;
}

void AudioEngine::UnregisterSoundSource()
{
}

void AudioEngine::UpdateSoundSource(const AudioHandle aHandle, const CU::Matrix4x4f& aMatrix)
{
    aMatrix, aHandle;
}

void AudioEngine::Impl::PushCommand(AudioCommand aCommand)
{
    int start1, size1, start2, size2;
    CommandFifo.prepareToWrite(1, start1, size1, start2, size2);
    if (size1 > 0) CommandBuffer[start1] = std::move(aCommand);
    CommandFifo.finishedWrite(size1);
}

void AudioEngine::Play2DSource(const AudioHandle aHandle)
{
    if (!myIsInitialized) return;

    AudioCommand cmd;
    cmd.Type = AudioCommand::Play;
    cmd.Handle = aHandle;

    myImpl->PushCommand(std::move(cmd));
}

AudioEngine::Impl::Impl()
{
    FormatManager.registerBasicFormats();
}

void AudioEngine::Impl::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels, float* const* outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext& context)
{
    context;
    numInputChannels;
    inputChannelData;


    // 1. DRAIN COMMANDS
    int start1, size1, start2, size2;
    CommandFifo.prepareToRead(CommandFifo.getNumReady(), start1, size1, start2, size2);

    for (int i = 0; i < size1; ++i)
    {
        const auto& cmd = CommandBuffer[start1 + i];

        switch (cmd.Type)
        {
        case AudioCommand::AddSource:
            // Direct access to the map member of 'this'
            AudioSources[cmd.Handle] = std::move(const_cast<AudioCommand&>(cmd).SourceData);
            break;

        case AudioCommand::Play:
            if (AudioSources.contains(cmd.Handle))
            {
                AudioSources[cmd.Handle]->ReadIndex = 0;
                AudioSources[cmd.Handle]->IsPlaying = true;
            }
            break;

        case AudioCommand::Stop:
            if (AudioSources.contains(cmd.Handle))
                AudioSources[cmd.Handle]->IsPlaying = false;
            break;
        }
    }
    CommandFifo.finishedRead(size1);

    // 2. MIXING
    juce::AudioBuffer<float> outputBuffer(const_cast<float**>(outputChannelData), numOutputChannels, numSamples);
    outputBuffer.clear();

    for (auto& [handle, sourcePtr] : AudioSources)
    {
        if (sourcePtr && sourcePtr->IsPlaying)
        {
            sourcePtr->Process(outputBuffer);
        }
    }
}

void AudioEngine::Impl::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    device;
}

void AudioEngine::Impl::audioDeviceStopped()
{
}
