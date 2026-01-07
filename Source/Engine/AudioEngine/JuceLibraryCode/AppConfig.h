#pragma once
#define JUCE_DONT_DECLARE_PROJECTINFO 1
#define JUCE_CHECK_PROTOCOL_WITH_BUILD_DATE 1

// Crucial for manual builds: Tells JUCE you are providing your own config
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1

#define JUCE_MODULE_AVAILABLE_juce_core 1
#define JUCE_MODULE_AVAILABLE_juce_events 1
#define JUCE_MODULE_AVAILABLE_juce_audio_basics 1
#define JUCE_MODULE_AVAILABLE_juce_audio_devices 1
#define JUCE_MODULE_AVAILABLE_juce_audio_formats 1
#define JUCE_MODULE_AVAILABLE_juce_dsp 1

// Disable Ogg/Vorbis to avoid conflicts with other engine libs
#define JUCE_USE_OGGVORBIS 0
#define JUCE_USE_FLAC 0
#define JUCE_USE_MP3AUDIOFORMAT 0
#define JUCE_USE_LAME_AUDIO_FORMAT 0

// Crucial: Prevent JUCE from including windows.h itself
// This stops JUCE and your Engine from fighting over Windows macros
#define JUCE_INCLUDE_PNGLIB_CODE 0
#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1

// Disable GUI dependencies (keeps the engine lightweight)
#define JUCE_WEB_BROWSER 0
#define JUCE_USE_CURL 0
#define JUCE_GUI_BASICS 0

// Audio/DSP Specific Flags
#define JUCE_USE_DIRECTSHOW 0     // Use modern APIs instead
#define JUCE_DIRECTSOUND 1        // Standard for Windows legacy support
#define JUCE_ASIO 0               // Highly recommended for pro audio (requires ASIO SDK)
#define JUCE_WASAPI 1             // Best low-latency native Windows API
#define JUCE_DSP_USE_INTEL_MKL 0  // Set to 1 only if using Intel's Math Kernel Library
