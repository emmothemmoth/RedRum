#include "Audioengine.pch.h"

#include "JuceLibraryCode/AppConfig.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
//#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h> 

#include "AudioEngine.h"
#include "RoomSimulator/RoomSimulator.h"

#include <unordered_map>
#include <optional>

#include <iostream>

struct AudioFile
{
    juce::AudioSampleBuffer Buffer;
    double SampleRate = 0;
};


struct AudioSource
{
    // We need a move constructor because unique_ptr can't be copied
    AudioSource(){}
    AudioSource(AudioSource&& other) noexcept = default;
    AudioSource& operator=(AudioSource&& other) noexcept = default;
    void Prepare(int numOutputChannels)
    {
        Interpolators.clear();
        Interpolators.resize(numOutputChannels);
        for (auto& interpolator : Interpolators)
        {
            interpolator = std::make_unique<juce::LagrangeInterpolator>();
        }
    }

    // Delete the copy constructor and assignment to satisfy the compiler
    AudioSource(const AudioSource&) = delete;
    AudioSource& operator=(const AudioSource&) = delete;
    void Process(juce::AudioBuffer<float>& outputBuffer)
    {
        if (!IsPlaying || CurrentSound == nullptr)
            return;

        auto& soundBuffer = CurrentSound->Buffer;

        const int numChannels = outputBuffer.getNumChannels();
        const int numSamples = outputBuffer.getNumSamples();
        const int soundLength = soundBuffer.getNumSamples();

        if (soundLength == 0)
            return;

        if (ReadIndex >= soundLength)
        {
            if (Loop) ReadIndex = 0;
            else { IsPlaying = false; return; }
        }

        // how many input samples interpolation may consume
        const int interpolationSafety = 4;
        int inputNeeded = static_cast<int>(numSamples * Ratio) + interpolationSafety;

        int samplesAvailable = soundLength - ReadIndex;

        int samplesToProcess = numSamples;

        if (!Loop && inputNeeded > samplesAvailable)
        {
            // clamp output so interpolator never reads past the buffer
            samplesToProcess = static_cast<int>((samplesAvailable - interpolationSafety) / Ratio);

            if (samplesToProcess <= 0)
            {
                IsPlaying = false;
                ReadIndex = 0;
                return;
            }
        }
        int usedCount = 0;
        for (int chan = 0; chan < numChannels; ++chan)
        {
            int sourceChan = chan % soundBuffer.getNumChannels();
            const float* sourcePtr = soundBuffer.getReadPointer(sourceChan, ReadIndex);
            float* destPtr = outputBuffer.getWritePointer(chan);

            usedCount = Interpolators[chan]->process(Ratio, sourcePtr, destPtr, samplesToProcess);

            // clear remaining buffer if clamped
            if (samplesToProcess < numSamples)
            {
                juce::FloatVectorOperations::clear(destPtr + samplesToProcess,
                    numSamples - samplesToProcess);
            }
        }

        ReadIndex += usedCount;

        if (ReadIndex >= soundLength)
        {
            if (Loop)
                ReadIndex %= soundLength;
            else
            {
                IsPlaying = false;
                ReadIndex = 0;
            }
        }
    }
    CommonUtilities::Matrix4x4f Transform;
    AudioFile* CurrentSound = nullptr;
    std::vector<std::unique_ptr<juce::LagrangeInterpolator>> Interpolators;
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

    enum { Play, Stop, Pause, StopAll, UpdateTransform, AddSource } Type;
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

    juce::ScopedJuceInitialiser_GUI juceInit;
	juce::AudioDeviceManager DeviceManager;
	std::unordered_map<AudioHandle, std::unique_ptr<AudioSource>> AudioSources;
	AudioHandle HandleCounter = 0;

    std::filesystem::path ContentPath;

    RoomSimulator Simulator;
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

void AudioEngine::InitListener(const CU::Matrix4x4f& aTransform)
{
    myImpl->Simulator.InitListener(aTransform);
}

std::optional<AudioHandle> AudioEngine::RegisterSoundSource(const std::filesystem::path& aFilePath)
{
    std::filesystem::path absoluteContentRoot = std::filesystem::weakly_canonical(myImpl->ContentPath);
    std::filesystem::path fullPath;

    if (aFilePath.is_absolute())
    {
        fullPath = std::filesystem::weakly_canonical(aFilePath);
    }
    else
    {
        std::string cleanPathStr = aFilePath.string();
        while (!cleanPathStr.empty() && (cleanPathStr.front() == '/' || cleanPathStr.front() == '\\'))
        {
            cleanPathStr.erase(0, 1);
        }
        std::filesystem::path cleanRelativePath(cleanPathStr);

        // Now operator/ will safely append "Audio/..." to "Content/"
        fullPath = std::filesystem::weakly_canonical(absoluteContentRoot / cleanRelativePath);
    }

    // Generate a path relative to your content root.
    std::filesystem::path relativeKey = std::filesystem::relative(fullPath, absoluteContentRoot);


    // 1. Load the file (Heavy Work - Main Thread)
    // ALWAYS pass the absolute 'fullPath' to JUCE so the OS can actually find it on disk.
    juce::File file(fullPath.wstring().c_str());
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
    newSource->Prepare(reader->numChannels);
    newSource->Ratio = newFile->SampleRate / hardwareSampleRate;
    newSource->CurrentSound = newFile.get();
    newSource->IsPlaying = false;

    // 3. Keep the data alive in our "Library"
    // Use the relativeKey so your engine's asset tracking remains 100% consistent, 
    // even if the user dragged in an absolute file path!
    myImpl->LoadedFiles[relativeKey.string()] = newFile;

    // 4. Send the command to the Audio Thread
    AudioCommand cmd;
    cmd.Type = AudioCommand::AddSource;
    cmd.Handle = handle;
    cmd.SourceData = std::move(newSource);
    myImpl->PushCommand(std::move(cmd));


    //TODO: Need to decouple handles for audio sources and emitters. There can be multiple emitters with the same audio source!!!
    
    return handle;
}

void AudioEngine::Impl::UnregisterSoundSource()
{
    //TODO: remove the file from impl
}

void AudioEngine::UnregisterSoundSource(const AudioHandle aHandle)
{
    myImpl->Simulator.UnregisterEmitter(aHandle);
}

std::optional<EmitterHandle> AudioEngine::RegisterAudioEmitter(AudioHandle aSourceHandle, const EmitterSettings& someSettings, const CU::Matrix4x4f& aTransform)
{
    aSourceHandle;
    someSettings;
    aTransform;
    EmitterHandle handle;
    //TODO: need to get the source buffer from the impl
    //auto handle = myImpl->Simulator.RegisterEmitter(BUFFER, someSettings, aTransform);
    //return handle;
    return handle;
}

std::optional<ObstacleHandle> AudioEngine::RegisterAudioObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform)
{
    return myImpl->Simulator.RegisterObstacle(someSettings, aCollider, aTransform);
}

void AudioEngine::UnregisterAudioObstacle(ObstacleHandle aHandle)
{
    myImpl->Simulator.UnregisterObstacle(aHandle);
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

void AudioEngine::Control2DSource(const AudioHandle aHandle, const AudiosourceControl aControltype)
{
    if (!myIsInitialized) return;

    AudioCommand cmd;
    switch (aControltype)
    {
    case AudiosourceControl::Play:
    {
        {
            AudioCommand stopAll;
            stopAll.Type = AudioCommand::StopAll;
            myImpl->PushCommand(std::move(stopAll));
        }
        cmd.Type = AudioCommand::Play;
        break;
    }
    case AudiosourceControl::Pause:
    {
        cmd.Type = AudioCommand::Pause;
        break;
    }
    case AudiosourceControl::Stop:
    {
        cmd.Type = AudioCommand::Stop;
        break;
    }
    default:
        return;
    }
    cmd.Handle = aHandle;

    myImpl->PushCommand(std::move(cmd));
}

AudioEngine::Impl::Impl()
{
    FormatManager.registerBasicFormats();
    AudioSources.reserve(64);
    juce::File exeFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    juce::File contentDir = exeFile.getParentDirectory().getChildFile("Content");

    juce::String absolutePath = contentDir.getFullPathName();
    ContentPath = absolutePath.toStdString();
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
        auto& cmd = CommandBuffer[start1 + i];

        switch (cmd.Type)
        {
        case AudioCommand::AddSource:
            AudioSources.emplace(cmd.Handle, std::move(cmd.SourceData));
            break;

        case AudioCommand::Play:
        {
            auto it = AudioSources.find(cmd.Handle);
            if (it != AudioSources.end())
            {
                auto& src = it->second;
                src->ReadIndex = 0;
                src->IsPlaying = true;
                for (auto& interpolator : src->Interpolators)
                {
                    interpolator->reset();
                }
            }
            break;
        }
        case AudioCommand::Pause:
        {
            auto it = AudioSources.find(cmd.Handle);
            if (it != AudioSources.end())
            {
                it->second->IsPlaying = false;
            }
            break;
        }
        case AudioCommand::Stop:
        {
            auto it = AudioSources.find(cmd.Handle);
            if (it != AudioSources.end())
            {
                auto& src = it->second;
                src->ReadIndex = 0;
                src->IsPlaying = false;
            }
            break;
        }
        case AudioCommand::StopAll:
        {
            for (auto& [handle, source] : AudioSources)
            {
                source->IsPlaying = false;
                source->ReadIndex = 0;
            }
            break;
        }
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
