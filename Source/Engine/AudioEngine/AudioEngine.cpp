#include "Audioengine.pch.h"

#include "JuceLibraryCode/AppConfig.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
//#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h> 

#include "AudioEngine.h"
#include "RoomSimulator/RoomSimulator.h"

#include "CommonUtilities/Intersection.hpp"

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
    void Prepare(int numOutputChannels, double hardwareSampleRate)
    {
        Interpolators.clear();
        Interpolators.resize(numOutputChannels);
        for (auto& interpolator : Interpolators)
        {
            interpolator = std::make_unique<juce::LagrangeInterpolator>();
        }
        juce::dsp::ProcessSpec spec{ hardwareSampleRate, 2048, static_cast<juce::uint32>(numOutputChannels) };
        OcclusionFilter.prepare(spec);
        OcclusionFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
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
    juce::dsp::StateVariableTPTFilter<float> OcclusionFilter;
    double Ratio = 1.0;
    int ReadIndex = 0;
    float TargetOcclusion = 0.0f;
    float CurrentOcclusion = 0.0f;
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
    int LoadedProbeIndices[2] = { -1,-1 };
    CU::Vector3f LastListenerRight = { 0, 0, 0 };

    std::vector<AcousticProbe> CachedGrid;
    std::atomic<double> SampleRate;
    bool IsRoomBaked = false;
    bool IsSimulationRunning = false;
    CU::Vector3f LiveListenerPos = { 0,0,0 };
    CU::Vector3f LiveListenerRight = { 0,0,0 };
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
                myImpl->LoadedProbeIndices[0] = -1;
                myImpl->LoadedProbeIndices[1] = -1;

                OnSimulationReady.Broadcast(0);
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
        currentRight.Normalize();

        myImpl->LiveListenerPos = listenerPos;
        myImpl->LiveListenerRight = currentRight;
    }
    for (auto& [handle, sourcePtr] : myImpl->AudioSources)
    {
        if (!sourcePtr || !sourcePtr->IsPlaying) continue;

        CU::Vector3f sourcePos = { sourcePtr->Transform(4,1), sourcePtr->Transform(4,2), sourcePtr->Transform(4,3) };
        CU::Vector3f dirToSource = sourcePos - listenerPos;
        float distanceToSource = dirToSource.Length();
        if (distanceToSource > 0.001f) dirToSource.Normalize();

        bool isOccluded = false;

        for (const auto& obstacle : myImpl->Simulator.GetObstaclesCopy())
        {
            CU::Vector4f localOrigin4 = obstacle.InverseTransform * CU::Vector4f(listenerPos.x, listenerPos.y, listenerPos.z, 1.0f);
            CU::Vector4f localDir4 = obstacle.InverseTransform * CU::Vector4f(dirToSource.x, dirToSource.y, dirToSource.z, 0.0f);
            CU::Vector3f localOrigin = { localOrigin4.x, localOrigin4.y, localOrigin4.z };
            CU::Vector3f localDir = { localDir4.x, localDir4.y, localDir4.z };

            if (std::holds_alternative<AABBCollider>(obstacle.Collider.Shape))
            {
                const auto& aabb = std::get<AABBCollider>(obstacle.Collider.Shape);
                float hitT;
                CU::Vector3f localNormal;

                if (CU::RaycastAABB(localOrigin, localDir, aabb.MinPoint, aabb.MaxPoint, hitT, localNormal))
                {
                    float worldT = hitT * localDir.Length();

                    if (worldT > 10.0f/*&& worldT < (distanceToSource - 10.0f)*/)
                    {
                        //isOccluded = true; //TODO!!
                        isOccluded = false; //TODO!!
                        break;
                    }
                }
            }
        }
        sourcePtr->TargetOcclusion = isOccluded ? 1.0f : 0.0f;
    }

    int topProbes[2] = { -1, -1 };
    float topDists[2] = { 999999.0f, 999999.0f };

    for (int i = 0; i < myImpl->CachedGrid.size(); ++i)
    {
        float dist = (myImpl->CachedGrid[i].Position - listenerPos).Length();
        if (dist < topDists[0])
        {
            topDists[1] = topDists[0]; topProbes[1] = topProbes[0];
            topDists[0] = dist;        topProbes[0] = i;
        }
        else if (dist < topDists[1])
        {
            topDists[1] = dist; topProbes[1] = i;
        }
    }

    if (topProbes[0] == -1) return;
    if (topProbes[1] == -1)
    {
        topProbes[1] = topProbes[0];
        topDists[1] = topDists[0];
    }

    // 2. Calculate Blend Weights (Inverse Distance)
    // Avoid div by zero if standing directly on a probe
    float totalDist = topDists[0] + topDists[1];
    float weight0 = (totalDist > 0.01f) ? (1.0f - (topDists[0] / totalDist)) : 1.0f;
    float weight1 = (totalDist > 0.01f) ? (1.0f - (topDists[1] / totalDist)) : 0.0f;

    // Force rotation updates (re-bake the IRs if user spins)
    bool rotationChanged = (myImpl->LastListenerRight.Dot(currentRight) < 0.998f);
    if (rotationChanged) myImpl->LastListenerRight = currentRight;

    // 3. Engine Assignment Logic
    for (int p = 0; p < 2; ++p)
    {
        int requiredProbe = topProbes[p];
        float requiredVolume = (p == 0) ? weight0 : weight1;

        // Check if this probe is already loaded in one of the engines
        int foundInEngine = -1;
        if (myImpl->LoadedProbeIndices[0] == requiredProbe) foundInEngine = 0;
        if (myImpl->LoadedProbeIndices[1] == requiredProbe) foundInEngine = 1;

        if (foundInEngine != -1)
        {
            // It's loaded! Just update its volume and optionally refresh rotation
            myImpl->ReverbVolumes[foundInEngine].setTargetValue(requiredVolume);
            if (rotationChanged)
            {
                myImpl->LoadProbeIR(myImpl->CachedGrid[requiredProbe], foundInEngine, currentRight);
            }
        }
        else
        {
            // It's NOT loaded. Find the engine that is NOT holding the other top probe
            int engineToOverwrite = (myImpl->LoadedProbeIndices[0] == topProbes[1 - p]) ? 1 : 0;

            myImpl->LoadProbeIR(myImpl->CachedGrid[requiredProbe], engineToOverwrite, currentRight);
            myImpl->LoadedProbeIndices[engineToOverwrite] = requiredProbe;
            myImpl->ReverbVolumes[engineToOverwrite].setTargetValue(requiredVolume);
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

    newSource->Prepare(numChannels, hardwareSampleRate);
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

        auto sourceIt = myImpl->AudioSources.find(aSourceHandle);
        if (sourceIt != myImpl->AudioSources.end())
        {
            sourceIt->second->Transform = aTransform;
        }

        return myImpl->Simulator.RegisterEmitter(sourceBuffer, sourceRate, someSettings, aTransform);
    }

    return std::nullopt;
}

void AudioEngine::UpdateAudioEmitter(const EmitterHandle aHandle, const EmitterSettings& someSettings, const CU::Matrix4x4f& aMatrix)
{
    myImpl->Simulator.UpdateEmitter(aHandle, someSettings, aMatrix);
    auto it = myImpl->AudioSources.find(aHandle);
    if (it != myImpl->AudioSources.end())
    {
        it->second->Transform = aMatrix;
    }
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

    // 1. Create 3 separate buffers for our frequency bands
    juce::AudioBuffer<float> lowBuffer(2, irLengthSamples);
    juce::AudioBuffer<float> midBuffer(2, irLengthSamples);
    juce::AudioBuffer<float> highBuffer(2, irLengthSamples);
    lowBuffer.clear(); midBuffer.clear(); highBuffer.clear();

    float rayLimitFloat = static_cast<float>(Simulator.GetRayLimit());
    float reverbBoost = 100.0f;

    // 2. Mix the Reflections into their respective frequency bands
    for (const auto& hit : activeProbe.Hits)
    {
        float delaySeconds = hit.Distance / 34300.0f;
        int delaySamples = static_cast<int>(delaySeconds * sampleRate);

        if (delaySamples >= irLengthSamples) continue;

        float pan = listenerRight.Dot(-hit.RayDirection); // Use live rotation!
        float livePanAngle = (pan + 1.0f) * 0.5f * (3.14159f * 0.5f);

        float leftPan = std::cos(livePanAngle);
        float rightPan = std::sin(livePanAngle);


        float baseGain = reverbBoost / rayLimitFloat;

        float highAirAbsorb = std::pow(0.9f, delaySeconds);
        float midAirAbsorb = std::pow(0.98f, delaySeconds);

        // Multiply the base gain by the 3-band absorption we got from the GPU
        float lowGain = hit.Power.x * baseGain;
        float midGain = hit.Power.y * baseGain * midAirAbsorb;
        float highGain = hit.Power.z * baseGain * highAirAbsorb;

        // Add to the respective band buffers
        lowBuffer.addSample(0, delaySamples, lowGain * leftPan);
        lowBuffer.addSample(1, delaySamples, lowGain * rightPan);

        midBuffer.addSample(0, delaySamples, midGain * leftPan);
        midBuffer.addSample(1, delaySamples, midGain * rightPan);

        highBuffer.addSample(0, delaySamples, highGain * leftPan);
        highBuffer.addSample(1, delaySamples, highGain * rightPan);
    }

    // 3. Apply physical EQ filters to the 3 Band Buffers
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = static_cast<double>(sampleRate);
    spec.maximumBlockSize = static_cast<juce::uint32>(irLengthSamples);
    spec.numChannels = 1;

    juce::dsp::IIR::Filter<float> lowPass[2];
    juce::dsp::IIR::Filter<float> midBand[2];
    juce::dsp::IIR::Filter<float> highPass[2];

    // Create the coefficients once
    auto lpCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 250.0f);
    auto mbCoeffs = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 1000.0f);
    auto hpCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 4000.0f);

    // Process Left (0) and Right (1) entirely independently
    for (int c = 0; c < 2; ++c)
    {
        lowPass[c].prepare(spec);
        midBand[c].prepare(spec);
        highPass[c].prepare(spec);

        lowPass[c].coefficients = lpCoeffs;
        midBand[c].coefficients = mbCoeffs;
        highPass[c].coefficients = hpCoeffs;

        // Wrap just this single channel in an AudioBlock
        juce::dsp::AudioBlock<float> lowChannel(lowBuffer.getArrayOfWritePointers() + c, 1, irLengthSamples);
        lowPass[c].process(juce::dsp::ProcessContextReplacing<float>(lowChannel));

        juce::dsp::AudioBlock<float> midChannel(midBuffer.getArrayOfWritePointers() + c, 1, irLengthSamples);
        midBand[c].process(juce::dsp::ProcessContextReplacing<float>(midChannel));

        juce::dsp::AudioBlock<float> highChannel(highBuffer.getArrayOfWritePointers() + c, 1, irLengthSamples);
        highPass[c].process(juce::dsp::ProcessContextReplacing<float>(highChannel));
    }
    // 4. Sum the filtered bands back into the final IR buffer
    juce::AudioBuffer<float> irBuffer(2, irLengthSamples);
    irBuffer.clear();
    for (int c = 0; c < 2; ++c)
    {
        irBuffer.addFrom(c, 0, lowBuffer, c, 0, irLengthSamples);
        irBuffer.addFrom(c, 0, midBuffer, c, 0, irLengthSamples);
        irBuffer.addFrom(c, 0, highBuffer, c, 0, irLengthSamples);
    }

    // 5. Hand the finalized Room Fingerprint to the Convolution Engine
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

    // We need a small temporary buffer to hold the raw source before we route it
    juce::AudioBuffer<float> tempSourceBuffer(numOutputChannels, numSamples);

    // 1. ROUTE AUDIO
    for (auto& [handle, sourcePtr] : AudioSources)
    {
        if (sourcePtr && sourcePtr->IsPlaying)
        {
            tempSourceBuffer.clear();
            sourcePtr->Process(tempSourceBuffer);

            if (IsSimulationRunning)
            {
                for (int c = 0; c < numOutputChannels; ++c)
                {
                    Dry3DBuffer.addFrom(c, 0, tempSourceBuffer, c, 0, numSamples);
                }

                // --- NEW: OCCLUSION DSP ---
                // 1. Smooth the occlusion value to prevent clicks (0.01 = smoothing speed)
                sourcePtr->CurrentOcclusion += (sourcePtr->TargetOcclusion - sourcePtr->CurrentOcclusion) * 0.1f;

                // 2. Map Occlusion (0.0 - 1.0) to a Frequency (20000Hz down to 800Hz)
                float cutoffFreq = 20000.0f - (sourcePtr->CurrentOcclusion * 19200.0f);
                sourcePtr->OcclusionFilter.setCutoffFrequency(cutoffFreq);

                // 3. Process the filter onto the Dry signal
                juce::dsp::AudioBlock<float> dryBlock(tempSourceBuffer);
                sourcePtr->OcclusionFilter.process(juce::dsp::ProcessContextReplacing<float>(dryBlock));

                // 4. Map Occlusion to a volume drop (drops to 30% volume when occluded)
                float occlusionVolumeMultiplier = 1.0f - (sourcePtr->CurrentOcclusion * 0.7f);


                // B. DRY PATH (Real-Time Distance & Panning)
                CU::Vector3f sourcePos = { sourcePtr->Transform(4,1), sourcePtr->Transform(4,2), sourcePtr->Transform(4,3) };
                CU::Vector3f dirToSource = sourcePos - LiveListenerPos;
                float distance = dirToSource.Length();

                const float minDistance = 200.0f;
                float attenuation = minDistance / (minDistance + 0.5f * (distance - minDistance));
                attenuation = std::min(1.0f, std::max(0.0f, attenuation));

                // Multiply inverse distance by occlusion muffle volume
                attenuation *= occlusionVolumeMultiplier;

                if (distance > 0.001f) dirToSource.Normalize();
                float pan = LiveListenerRight.Dot(dirToSource);
                float panAngle = (pan + 1.0f) * 0.5f * (3.14159f * 0.5f);

                float leftGain = attenuation * std::cos(panAngle);
                float rightGain = attenuation * std::sin(panAngle);

                outputBuffer.addFrom(0, 0, tempSourceBuffer, 0, 0, numSamples, leftGain);
                outputBuffer.addFrom(1, 0, tempSourceBuffer, 1, 0, numSamples, rightGain);
            }
            else
            {
                // PREVIEW MODE (2D Inspector)
                for (int c = 0; c < numOutputChannels; ++c)
                {
                    outputBuffer.addFrom(c, 0, tempSourceBuffer, c, 0, numSamples);
                }
            }
        }
    }

    bool hasLoadedProbes = (LoadedProbeIndices[0] != -1 || LoadedProbeIndices[1] != -1);

    if (IsSimulationRunning && IsRoomBaked && hasLoadedProbes)
    {
        for (int c = 0; c < numOutputChannels; ++c)
        {
            TempBufferA.copyFrom(c, 0, Dry3DBuffer, c, 0, numSamples);
            TempBufferB.copyFrom(c, 0, Dry3DBuffer, c, 0, numSamples);
        }

        // Process Convolution A (Only if it has a loaded probe)
        if (LoadedProbeIndices[0] != -1)
        {
            juce::dsp::AudioBlock<float> blockA(TempBufferA);
            Reverbs[0].process(juce::dsp::ProcessContextReplacing<float>(blockA));
        }
        else
        {
            TempBufferA.clear();
        }

        // Process Convolution B (Only if it has a loaded probe)
        if (LoadedProbeIndices[1] != -1)
        {
            juce::dsp::AudioBlock<float> blockB(TempBufferB);
            Reverbs[1].process(juce::dsp::ProcessContextReplacing<float>(blockB));
        }
        else
        {
            TempBufferB.clear();
        }

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float volA = ReverbVolumes[0].getNextValue();
            float volB = ReverbVolumes[1].getNextValue();

            for (int c = 0; c < numOutputChannels; ++c)
            {
                float wetSample = (TempBufferA.getSample(c, sample) * volA) +
                    (TempBufferB.getSample(c, sample) * volB);

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
