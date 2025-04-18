//
// Created by ns on 1/12/25.
//

#include "EffectsChannel.h"


namespace Gj {
namespace Audio {
namespace Effects {

using namespace Steinberg;

EffectsChannel::EffectsChannel(
	AppState* gAppState,
	std::shared_ptr<JackClient> jackClient,
	const int index
	)
	: gAppState(gAppState)
	, jackClient(jackClient)
	, index(index)
	, channel({ 1.0f, 0.0f })
  {

  Logging::write(
  	Info,
  	"EffectsChannel::ctor",
  	"Instantiating EffectsChannel: " + std::to_string(index)
  );
}

EffectsChannel::~EffectsChannel() {
  Logging::write(
  	Info,
  	"EffectsChannel::dtor",
  	"Destroying EffectsChannel: " + std::to_string(index)
  );

	Logging::write(
  	Info,
  	"EffectsChannel::dtor",
  	"Destroyed EffectsChannel: " + std::to_string(index)
  );
}

bool EffectsChannel::addEffect(const std::string& effectPath) {
	Logging::write(
		Info,
		"EffectsChannel::addEffect",
		"Adding effect: " + effectPath + " to channel " + std::to_string(index)
	);

  auto effect =
  	std::make_unique<Vst3::Plugin>(
  		effectPath,
  		gAppState,
  		jackClient
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
