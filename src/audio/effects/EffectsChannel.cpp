//
// Created by ns on 1/12/25.
//

#include "EffectsChannel.h"


namespace Gj {
namespace Audio {
namespace Effects {

using namespace Steinberg;

EffectsChannel::EffectsChannel(AppState* gAppState, const int index, float** inputBuffers)
  : gAppState(gAppState)
	, index(index)
	, inputBuffers(inputBuffers)
	, buffersA()
	, buffersB()
  , channel({ 1.0f, 0.0f })
  {

  Logging::write(
  	Info,
  	"EffectsChannel::ctor",
  	"Instantiating EffectsChannel: " + std::to_string(index)
  );

	allocateBuffers();

	Logging::write(
		Info,
		"EffectsChannel::ctor",
		"Allocated input buffers for EffectsChannel: " + std::to_string(index)
	);
}

EffectsChannel::~EffectsChannel() {
	freeBuffers();
}

void EffectsChannel::allocateBuffers() {
	buffersA = static_cast<float**>(
		malloc(2 * gAppState->audioFramesPerBuffer * sizeof(float))
	);
	buffersB = static_cast<float**>(
		malloc(2 * gAppState->audioFramesPerBuffer * sizeof(float))
	);

	if (buffersA == nullptr || buffersB == nullptr) {
		Logging::write(
			Error,
			"EffectsChannel::allocateBuffers",
			"Unable to allocate memory for buffersIn or buffersOut."
		);
		throw std::runtime_error ("Unable to allocate memory for EffectsChannel buffers.");
	}

	for (int c = 0; c < 2; c++) {
		buffersA[c] = new float[gAppState->audioFramesPerBuffer];
		buffersB[c] = new float[gAppState->audioFramesPerBuffer];
	}
}

void EffectsChannel::freeBuffers() const {
	try {
		for (int i = 0; i < 2; i++) {
			delete buffersA[i];
			delete buffersB[i];
		}

		free(buffersA);
		free(buffersB);
	} catch (...) {
		Logging::write(
			Error,
			"EffectsChannel::freeBuffers",
			"An error occurred while attempting to free buffers for EffectsChannel " + std::to_string(index)
		);
	}
}

float** EffectsChannel::determineInputBuffers(const int index) const {
	if (index == 0)
		return inputBuffers;

	if (index % 2 == 0)
		return buffersB;

	// index % 2 == 1
	return buffersA;
}

// output for plugin at index
float** EffectsChannel::determineOutputBuffers(const int index) const {
	if (index % 2 == 0)
		return buffersA;

	return buffersB;
}

// output read by Mixer after processing
float** EffectsChannel::getBuffersWriteOut() const {
	if (vst3Plugins.size() % 2 == 0)
		return buffersB;

	return buffersA;
}

bool EffectsChannel::addEffect(const std::string& effectPath) {
	Logging::write(
		Info,
		"EffectsChannel::addEffect",
		"Adding effect: " + effectPath + " to channel " + std::to_string(index)
	);

	const int effectIndex = static_cast<int>(vst3Plugins.size());
	float** in = determineInputBuffers(effectIndex);
	float** out = determineOutputBuffers(effectIndex);

  auto effect =
  	std::make_unique<Vst3::Plugin>(
  		effectPath,
  		gAppState,
  		in,
  		out
  	);

	Logging::write(
		Info,
		"EffectsChannel::addEffect",
		"Instantiated plugin " + effect->path + " for channel " + std::to_string(index)
	);

  const FUnknownPtr<Vst::IAudioProcessor> processor = effect->getProcesser();
  // int latencySamples = processor->getLatencySamples();
  // incorporateLatencySamples(latencySamples);

  if (!processor->canProcessSampleSize(gAppState->audioFramesPerBuffer)) {
		Logging::write(
			Warning,
			"EffectsChannel::addEffect",
			"Processor for " + effectPath + " on channel " + std::to_string(index) + " cannot process sample size " + std::to_string(gAppState->audioFramesPerBuffer)
		);
  	return false;
  }

  // Vst::BusDirection busDirection;
  // int32 index;
  // Vst::SpeakerArrangement arrangement;
  // processor->getBusArrangement(busDirection, index, arrangement);

  const int32 maxSamplesPerBlock = gAppState->audioFramesPerBuffer;
  Vst::ProcessSetup setup = {
    Vst::kRealtime,
    Vst::kSample64,
    maxSamplesPerBlock,
    44100.0
  };
  processor->setupProcessing(setup);
  vst3Plugins.push_back(std::move(effect));

	Logging::write(
		Info,
		"EffectsChannel::addEffect",
		"Effect " + effectPath + " added on channel " + std::to_string(index)
	);

	return true;
}

void EffectsChannel::setSampleRate(int sampleRate) const {
	for (auto&& plugin : vst3Plugins) {
		plugin->audioHost->audioClient->setSamplerate(sampleRate);
		plugin->audioHost->audioClient->setBlockSize(gAppState->audioFramesPerBuffer);
	}
}

void EffectsChannel::process() const {
	for (auto&& plugin : vst3Plugins) {
		const auto audioHost = plugin->audioHost;
		audioHost->audioClient->process(audioHost->buffers, gAppState->audioFramesPerBuffer);
	}
}

int EffectsChannel::effectCount() const {
	return static_cast<int>(vst3Plugins.size());
}

void EffectsChannel::initEditorHosts(const std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const {
	Logging::write(
		Info,
		"EffectsChannel::initEditorHosts",
		"Initializing Editor Hosts on channel " + std::to_string(index)
	);
	int i = 0;
	for (const auto& plugin : vst3Plugins) {
		Logging::write(
			Info,
			"EffectsChannel::initEditorHosts",
			"Initializing Editor Host for effect " + plugin->path + " on channel " + std::to_string(index)
		);
		plugin->initEditorHost(vstWindows.at(i));
		i++;
	}
}

void EffectsChannel::terminateEditorHosts() const {
	for (auto&& plugin : vst3Plugins) {
		plugin->terminateEditorHost();
	}
}


} // Effects
} // Audio
} // Gj
