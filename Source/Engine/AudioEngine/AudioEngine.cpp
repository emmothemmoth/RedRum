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
    std::shared_ptr<AudioFile> CurrentSound;
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

    enum { PlayScene, Play, Stop, Pause, StopAll, AddSource, UpdateSource } Type;
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

    void LoadProbeIR(const AcousticProbe& activeProbe, int targetReverbIndex, const CU::Vector3f& listenerRight);

    void PushCommand(AudioCommand aCommand);

    static constexpr int QueueSize = 128;
    juce::AbstractFifo CommandFifo{ QueueSize };
    AudioCommand CommandBuffer[QueueSize];

    std::vector<AudioSource> ActiveVoices;

    juce::AudioFormatManager FormatManager;
    std::unordered_map<std::string, std::shared_ptr<AudioFile>> LoadedFiles;
    std::unordered_map<AudioHandle, std::shared_ptr<AudioFile>> FileRegistry;

    juce::ScopedJuceInitialiser_GUI juceInit;
	juce::AudioDeviceManager DeviceManager;
	std::unordered_map<AudioHandle, std::unique_ptr<AudioSource>> AudioSources;
    juce::AudioBuffer<float> Dry3DBuffer;
    std::vector<AudioHandle> SceneEmitterHandles;
	AudioHandle HandleCounter = 0;

    std::filesystem::path ContentPath;

    RoomSimulator Simulator;
    std::optional<AudioHandle> BakedRoomHandle = std::nullopt;

    juce::dsp::Convolution Reverbs[2];
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> ReverbVolumes[2];
    juce::AudioBuffer<float> TempBufferA;
    juce::AudioBuffer<float> TempBufferB;
    int ActiveReverbIdx = 0;
    CU::Vector3f LastListenerRight = { 0, 0, 0 };

    std::vector<AcousticProbe> CachedGrid;
    int ActiveProbeIndex = -1;
    std::atomic<double> SampleRate;
    bool IsRoomBaked = false;
    bool IsSimulationRunning = false;
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
        myImpl->SampleRate = mySampleRate;
        myIsInitialized = true;

        myImpl->Simulator.OnScoutBatchReady.AddLambda([this](EmitterHandle handle, std::vector<VisualRayPath> paths)
            {
                OnVisualRaysReady.Broadcast(handle, std::move(paths));
            });

        myImpl->Simulator.OnMegaBakeComplete.AddLambda([this](std::vector<AcousticProbe> bakedGrid)
            {
                myImpl->CachedGrid = std::move(bakedGrid);
                myImpl->IsRoomBaked = true;
                myImpl->ActiveProbeIndex = -1; // Force an audio update on the next frame
            });

        myImpl->DeviceManager.initialiseWithDefaultDevices(0, 2);
        myImpl->DeviceManager.addAudioCallback(myImpl.get());
    }
}

void AudioEngine::Update()
{
    myImpl->Simulator.Update();

    if (!myImpl->IsRoomBaked || myImpl->CachedGrid.empty())
        return;

    CU::Vector3f listenerPos;
    CU::Vector3f currentRight;
    {
        auto transform = myImpl->Simulator.GetListenerTransform();
        listenerPos = { transform(4,1), transform(4,2), transform(4,3) };
        currentRight = { transform(1,1), transform(1,2), transform(1,3) };
    }

    int nearestIdx = -1;
    float minDist = 999999999.0f;

    for (int i = 0; i < myImpl->CachedGrid.size(); ++i)
    {
        float dist = (myImpl->CachedGrid[i].Position - listenerPos).Length();
        if (dist < minDist)
        {
            minDist = dist;
            nearestIdx = i;
        }
    }

    bool zoneChanged = (nearestIdx != -1 && nearestIdx != myImpl->ActiveProbeIndex);

    // Check if player rotated more than ~3.5 degrees
    bool rotationChanged = (myImpl->LastListenerRight.Dot(currentRight) < 0.998f);

    if (zoneChanged || (rotationChanged && myImpl->ActiveProbeIndex != -1))
    {
        bool wasFirstTime = (myImpl->ActiveProbeIndex == -1);

        myImpl->ActiveProbeIndex = nearestIdx;
        myImpl->LastListenerRight = currentRight; // Save rotation

        int inactiveIdx = (myImpl->ActiveReverbIdx == 0) ? 1 : 0;

        // Pass the live rotation into the IR generator!
        myImpl->LoadProbeIR(myImpl->CachedGrid[nearestIdx], inactiveIdx, currentRight);

        myImpl->ReverbVolumes[myImpl->ActiveReverbIdx].setTargetValue(0.0f);
        myImpl->ReverbVolumes[inactiveIdx].setTargetValue(1.0f);

        myImpl->ActiveReverbIdx = inactiveIdx;

        if (wasFirstTime)
        {
            OnSimulationReady.Broadcast(0);
        }
    }
}

void AudioEngine::InitListener(const CU::Matrix4x4f& aTransform)
{
    myImpl->Simulator.InitListener(aTransform);
}

void AudioEngine::UpdateListener(const CU::Matrix4x4f& aTransform)
{
    myImpl->Simulator.UpdateListener(aTransform);
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
        fullPath = std::filesystem::weakly_canonical(absoluteContentRoot / cleanRelativePath);
    }

    std::filesystem::path relativeKey = std::filesystem::relative(fullPath, absoluteContentRoot);

    // 2. OPEN FILE
    juce::File file(fullPath.wstring().c_str());
    std::unique_ptr<juce::AudioFormatReader> reader(myImpl->FormatManager.createReaderFor(file));

    if (reader == nullptr) return std::nullopt;

    // 3. DEFINE GLOBAL STANDARD
    const double& globalEditorRate = mySampleRate;
    auto newFile = std::make_shared<AudioFile>();
    newFile->SampleRate = globalEditorRate;

    int originalLength = (int)reader->lengthInSamples;
    int numChannels = (int)reader->numChannels;

    // 4. RESAMPLING LOGIC
    if (std::abs(reader->sampleRate - globalEditorRate) < 0.1)
    {
        // No conversion needed, just read directly
        newFile->Buffer.setSize(numChannels, originalLength);
        reader->read(&newFile->Buffer, 0, originalLength, 0, true, true);
    }
    else
    {
        // Calculate new length: (OriginalSamples * TargetRate) / SourceRate
        double resampleRatio = reader->sampleRate / globalEditorRate;
        int resampledLength = static_cast<int>(originalLength * (globalEditorRate / reader->sampleRate));

        newFile->Buffer.setSize(numChannels, resampledLength);

        // Temporary buffer to hold the raw file data before conversion
        juce::AudioBuffer<float> rawFileBuffer(numChannels, originalLength);
        reader->read(&rawFileBuffer, 0, originalLength, 0, true, true);

        // Perform interpolation per channel
        for (int chan = 0; chan < numChannels; ++chan)
        {
            juce::LagrangeInterpolator resampler;
            resampler.process(resampleRatio,
                rawFileBuffer.getReadPointer(chan),
                newFile->Buffer.getWritePointer(chan),
                resampledLength);
        }
    }

    // 5. PREPARE VOICE FOR PLAYBACK
    AudioHandle handle = myImpl->HandleCounter++;
    auto newSource = std::make_unique<AudioSource>();

    // We still need to check the hardware rate here.
    // If the editor is 48k but the headphones are 44.1k, we need a ratio of 1.088
    auto* device = myImpl->DeviceManager.getCurrentAudioDevice();
    double hardwareSampleRate = device ? device->getCurrentSampleRate() : 44100.0;

    newSource->Prepare(numChannels);
    newSource->Ratio = globalEditorRate / hardwareSampleRate; // Standardized Ratio
    newSource->CurrentSound = newFile;
    newSource->IsPlaying = false;

    // 6. REGISTRY & COMMAND
    myImpl->LoadedFiles[relativeKey.string()] = newFile;
    myImpl->FileRegistry[handle] = newFile;

    AudioCommand cmd;
    cmd.Type = AudioCommand::AddSource;
    cmd.Handle = handle;
    cmd.SourceData = std::move(newSource);
    myImpl->PushCommand(std::move(cmd));

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
    auto it = myImpl->FileRegistry.find(aSourceHandle);

    if (it != myImpl->FileRegistry.end() && it->second != nullptr)
    {
        myImpl->SceneEmitterHandles.push_back(aSourceHandle);
        const juce::AudioBuffer<float>* sourceBuffer = &it->second->Buffer;
        float sourceRate = static_cast<float>(it->second->SampleRate);

        return myImpl->Simulator.RegisterEmitter(sourceBuffer, sourceRate, someSettings, aTransform);
    }

    return std::nullopt;
}

void AudioEngine::UpdateAudioEmitter(const EmitterHandle aHandle, const EmitterSettings& someSettings, const CU::Matrix4x4f& aMatrix)
{
    myImpl->Simulator.UpdateEmitter(aHandle, someSettings, aMatrix);
}

void AudioEngine::UnregisterEmitter(const EmitterHandle aHandle)
{
    for (int index = (int)myImpl->SceneEmitterHandles.size() - 1; index >= 0; --index)
    {
        if (myImpl->SceneEmitterHandles.at(index) == aHandle)
        {
            myImpl->SceneEmitterHandles.erase(myImpl->SceneEmitterHandles.begin() + index);
            break;
        }
    }
    myImpl->Simulator.UnregisterEmitter(aHandle);
}

std::optional<ObstacleHandle> AudioEngine::RegisterAudioObstacle(const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform)
{
    return myImpl->Simulator.RegisterObstacle(someSettings, aCollider, aTransform);
}

void AudioEngine::UpdateAudioObstacle(ObstacleHandle aHandle, const AbsorberSettings& someSettings, const Collider& aCollider, const CU::Matrix4x4f& aTransform)
{
    myImpl->Simulator.UpdateObstacle(aHandle, someSettings, aCollider, aTransform);
}

void AudioEngine::UnregisterAudioObstacle(ObstacleHandle aHandle)
{
    myImpl->Simulator.UnregisterObstacle(aHandle);
}

void AudioEngine::Impl::LoadProbeIR(const AcousticProbe& activeProbe, int targetReverbIndex, const CU::Vector3f& listenerRight)
{
    int sampleRate = static_cast<int>(Simulator.GetRayLimit() > 0 ? 48000 : 48000);
    int irLengthSamples = sampleRate * 2; // 2 seconds max reverb tail

    juce::AudioBuffer<float> irBuffer(2, irLengthSamples);
    irBuffer.clear();

    const float minDistance = 200.0f;
    const float rolloffFactor = 0.5f;
    float rayLimitFloat = static_cast<float>(Simulator.GetRayLimit());

    // 1. WE DELETED THE FAKE DRY SOUND HERE
    // The physics engine will naturally provide the direct hits from the raycaster!

    // 2. Mix the Reflections (The "Wet" Tail)
    for (const auto& hit : activeProbe.Hits)
    {
        float delaySeconds = hit.Distance / 34300.0f;
        int delaySamples = static_cast<int>(delaySeconds * sampleRate);

        if (delaySamples >= irLengthSamples) continue;

        float attenuation = minDistance / (minDistance + rolloffFactor * (hit.Distance - minDistance));
        float clampedAtten = (attenuation < 1.0f) ? attenuation : 1.0f;

        // --- THE ENERGY FIX ---
        // A single-sample impulse needs significantly more amplitude than continuous audio.
        // We multiply by a "Reverb Boost" factor (e.g., 500.0f) so it's actually audible.
        // You can tweak this 500.0f up or down to act as a global "Wetness" knob!
        float reverbBoost = 300.0f;
        float finalGain = (clampedAtten * hit.Power * reverbBoost) / rayLimitFloat;

        float pan = listenerRight.Dot(-hit.RayDirection); // Use live rotation!
        float livePanAngle = (pan + 1.0f) * 0.5f * (3.14159f * 0.5f);

        float leftGain = std::cos(livePanAngle) * finalGain;
        float rightGain = std::sin(livePanAngle) * finalGain;

        irBuffer.addSample(0, delaySamples, leftGain);
        irBuffer.addSample(1, delaySamples, rightGain);
    }

    // 3. Hand the Room Fingerprint to the Convolution Engine
    Reverbs[targetReverbIndex].loadImpulseResponse(
        std::move(irBuffer),
        sampleRate,
        juce::dsp::Convolution::Stereo::yes,
        juce::dsp::Convolution::Trim::no,
        juce::dsp::Convolution::Normalise::no
    );
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

void AudioEngine::ControlRoomPlayback(AudiosourceControl aControltype)
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
        cmd.Type = AudioCommand::PlayScene;
        break;
    }
    case AudiosourceControl::Pause:
    {
        //TODO:
        cmd.Type = AudioCommand::Pause;
        return;
    }
    case AudiosourceControl::Stop:
    {
        cmd.Type = AudioCommand::StopAll;
        break;
    }
    default:
        return;
    }

    myImpl->PushCommand(std::move(cmd));
}

void AudioEngine::StartRoomSimulation()
{
    OnSimulationStarted.Broadcast();
    myImpl->Simulator.Simulate();
}

void AudioEngine::SetSampleRate(const double& aSampleRate)
{
    mySampleRate = aSampleRate;
    myImpl->SampleRate = aSampleRate;
    myImpl->Simulator.SetBakeRate(aSampleRate);
}

void AudioEngine::SetBakeMode(BakeMode aMode)
{
    switch (aMode)
    {
    case BakeMode::Preview_Fast:
        myImpl->Simulator.SetRayLimit(500);
        break;
    case BakeMode::Preview:
        myImpl->Simulator.SetRayLimit(500);
        break;
    case BakeMode::Export:
        myImpl->Simulator.SetRayLimit(50000);
        break;
    default:
        break;
    }
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
            IsSimulationRunning = false;
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
        case AudioCommand::PlayScene:
        {
            IsSimulationRunning = true;
            for (auto& [handle, source] : AudioSources) source->IsPlaying = false;
            for (AudioHandle emitterHandle : SceneEmitterHandles)
            {
                auto it = AudioSources.find(emitterHandle);
                if (it != AudioSources.end())
                {
                    it->second->ReadIndex = 0;
                    it->second->IsPlaying = true;
                    for (auto& interpolator : it->second->Interpolators) interpolator->reset();
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
            IsSimulationRunning = false;
            for (auto& [handle, source] : AudioSources)
            {
                source->IsPlaying = false;
                source->ReadIndex = 0;
            }
            break;
        }
        case AudioCommand::UpdateSource:
        {
            auto it = AudioSources.find(cmd.Handle);
            if (it != AudioSources.end())
            {
                bool wasPlaying = AudioSources[cmd.Handle]->IsPlaying;

                AudioSources[cmd.Handle] = std::move(cmd.SourceData);

                AudioSources[cmd.Handle]->IsPlaying = wasPlaying;
            }
            break;
        }
        }
    }
    CommandFifo.finishedRead(size1);

    juce::AudioBuffer<float> outputBuffer(const_cast<float**>(outputChannelData), numOutputChannels, numSamples);
    outputBuffer.clear();

    // Clear our 3D routing bus
    for (int c = 0; c < numOutputChannels; ++c)
    {
        Dry3DBuffer.clear(c, 0, numSamples);
    }

    // 1. ROUTE AUDIO
    for (auto& [handle, sourcePtr] : AudioSources)
    {
        if (sourcePtr && sourcePtr->IsPlaying)
        {
            if (IsSimulationRunning)
            {
                sourcePtr->Process(Dry3DBuffer); // Send to Reverb
            }
            else
            {
                sourcePtr->Process(outputBuffer); // Send directly to Speakers!
            }
        }
    }

    if (IsRoomBaked && ActiveProbeIndex != -1)
    {
        // 1. Clear ONLY the chunk of the temp buffers we are about to use
        // (Since block sizes fluctuate, we only clear 'numSamples')
        for (int c = 0; c < numOutputChannels; ++c)
        {
            // FIX A: Copy from Dry3DBuffer, not outputBuffer!
            TempBufferA.copyFrom(c, 0, Dry3DBuffer, c, 0, numSamples);
            TempBufferB.copyFrom(c, 0, Dry3DBuffer, c, 0, numSamples);
        }

        // 2. Process Convolution A
        juce::dsp::AudioBlock<float> blockA(TempBufferA.getArrayOfWritePointers(), numOutputChannels, numSamples);
        juce::dsp::ProcessContextReplacing<float> contextA(blockA);
        Reverbs[0].process(contextA);

        // 3. Process Convolution B
        juce::dsp::AudioBlock<float> blockB(TempBufferB.getArrayOfWritePointers(), numOutputChannels, numSamples);
        juce::dsp::ProcessContextReplacing<float> contextB(blockB);
        Reverbs[1].process(contextB);


        // 4. Apply smoothed crossfade volumes and sum to output
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float volA = ReverbVolumes[0].getNextValue();
            float volB = ReverbVolumes[1].getNextValue();

            for (int c = 0; c < numOutputChannels; ++c)
            {
                float wetSample = (TempBufferA.getSample(c, sample) * volA) +
                    (TempBufferB.getSample(c, sample) * volB);

                // ADD to the output buffer so we don't overwrite the 2D audio!
                outputBuffer.addSample(c, sample, wetSample);
            }
        }
    }
}

void AudioEngine::Impl::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    if (device != nullptr)
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = device->getCurrentSampleRate();
        spec.maximumBlockSize = device->getCurrentBufferSizeSamples();
        spec.numChannels = 2;

        Reverbs[0].prepare(spec);
        Reverbs[1].prepare(spec);

        // Set a 50ms crossfade
        ReverbVolumes[0].reset(spec.sampleRate, 0.05);
        ReverbVolumes[1].reset(spec.sampleRate, 0.05);

        ReverbVolumes[0].setCurrentAndTargetValue(1.0f);
        ReverbVolumes[1].setCurrentAndTargetValue(0.0f);

        TempBufferA.setSize(2, spec.maximumBlockSize);
        TempBufferB.setSize(2, spec.maximumBlockSize);
        Dry3DBuffer.setSize(2, spec.maximumBlockSize);
    }
}

void AudioEngine::Impl::audioDeviceStopped()
{
}
