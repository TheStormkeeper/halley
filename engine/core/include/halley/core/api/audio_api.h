#pragma once

#include <functional>
#include <gsl/gsl>
#include <memory>
#include <vector>
#include "halley/text/halleystring.h"

namespace Halley
{
	class AudioClip;

	class AudioDevice
	{
	public:
		virtual ~AudioDevice() {}
		virtual String getName() const = 0;
	};

	struct alignas(64) AudioSamplePack
	{
		std::array<float, 16> samples; // AVX-512 friendly
	};

	using AudioCallback = std::function<void(gsl::span<AudioSamplePack> span)>;

	enum class AudioSampleFormat
	{
		Undefined,
		Int16,
		Int32,
		Float
	};

	class AudioSpec
	{
	public:
		int sampleRate;
		int numChannels;
		int bufferSize;
		AudioSampleFormat format;

		AudioSpec() {}
		AudioSpec(int sampleRate, int numChannels, int bufferSize, AudioSampleFormat format)
			: sampleRate(sampleRate)
			, numChannels(numChannels)
			, bufferSize(bufferSize)
			, format(format)
		{}
	};

	class AudioOutputAPI
	{
	public:
		virtual ~AudioOutputAPI() {}

		virtual Vector<std::unique_ptr<const AudioDevice>> getAudioDevices() = 0;
		virtual AudioSpec openAudioDevice(const AudioSpec& requestedFormat, AudioCallback callback, const AudioDevice* device = nullptr) = 0;
		virtual void closeAudioDevice() = 0;

		virtual void startPlayback() = 0;
		virtual void stopPlayback() = 0;
	};

	class AudioPlaybackAPI
	{
	public:
		virtual ~AudioPlaybackAPI() {}

		virtual void playUI(std::shared_ptr<AudioClip> clip, float volume = 1.0f, float pan = 0.5f) = 0;
	};

	class AudioAPI : public AudioPlaybackAPI
	{
	public:
		virtual ~AudioAPI() {}

		virtual Vector<std::unique_ptr<const AudioDevice>> getAudioDevices() = 0;
		virtual void startPlayback(int deviceNumber = 0) = 0;
		virtual void stopPlayback() = 0;
	};
}
