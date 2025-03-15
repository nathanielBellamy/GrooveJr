//
// Created by ns on 1/12/25.
//

#include "EffectsChannel.h"


namespace Gj {
namespace Audio {
namespace Effects {

using namespace Steinberg;

EffectsChannel::EffectsChannel(AppState* gAppState, int index, float** inputBuffers)
  : gAppState(gAppState)
	, index(index)
	, inputBuffers(inputBuffers)
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

  for (const auto plugin : vst3Plugins) {
    delete plugin;
  }
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
	if (index == 0) {
		return inputBuffers;
	} else if (index % 2 == 0) {
		return buffersB;
	} else if (index % 2 == 1) {
		return buffersA;
	}
}

float** EffectsChannel::determineOutputBuffers(const int index) const {
	if (index % 2 == 0) {
		return buffersA;
	} else if (index % 2 == 1) {
		return buffersB;
	}
}

bool EffectsChannel::addEffect(const std::string& effectPath) {
	const int effectIndex = vst3Plugins.size();
	float** in = determineInputBuffers(effectIndex);
	float** out = determineOutputBuffers(effectIndex);
  const auto effect =
  	std::make_unique<Vst3::Plugin>(
  		effectPath,
  		gAppState,
  		in,
  		out
  	);

  const FUnknownPtr<Vst::IAudioProcessor> processor = effect->getProcesser();
  // int latencySamples = processor->getLatencySamples();
  // incorporateLatencySamples(latencySamples);

  if (!processor->canProcessSampleSize(gAppState->audioFramesPerBuffer)) {
    std::cout << "Mixer: " << effectPath << " Unable to process sample size" << std::endl;
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

  effect->setAudioFramesPerBuffer(gAppState->audioFramesPerBuffer);
  effect->allocateBuffers();

  vst3Plugins.push_back(effect);
  return chainBuffers();
}

bool EffectsChannel::chainBuffers() const {
  vst3Plugins.front()->audioHost->buffers.inputs = inputBuffers;

  for (int i = 1; i < vst3Plugins.size(); ++i) {
    const auto currentPlugin = vst3Plugins.at(i);
    const auto previousPlugin = vst3Plugins.at(i-1);
    const auto toFree = currentPlugin->audioHost->buffers.inputs;
    currentPlugin->audioHost->buffers.inputs = previousPlugin->audioHost->buffers.outputs;
    for (int j = 0; j < 2; j++) {
    	delete toFree[j];
    }
    free(toFree);
  }

  return true;
}

bool EffectsChannel::unchainBuffers() const {
  for (int i = 1; i < vst3Plugins.size(); ++i) {
  	vst3Plugins.at(i)->audioHost->allocateInputBuffers();
  }

  return true;
}

} // Effects
} // Audio
} // Gj
