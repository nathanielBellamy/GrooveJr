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
  {

  Logging::write(
  	Info,
  	"Audio::EffectsChannel::ctor",
  	"Instantiating EffectsChannel: " + std::to_string(index)
  );
}

EffectsChannel::EffectsChannel(
	AppState* gAppState,
	std::shared_ptr<JackClient> jackClient,
	const Db::ChannelEntity& channelEntity
	)
	: gAppState(gAppState)
	, jackClient(jackClient)
	, index(channelEntity.channelIndex)
	, name(channelEntity.name)
  {

	channel.gain.store(channelEntity.gain);
	channel.mute.store(channelEntity.mute);
	channel.solo.store(channelEntity.solo);
	channel.pan.store(channelEntity.pan);
	channel.gainL.store(channelEntity.gainL);
	channel.gainR.store(channelEntity.gainR);
	channel.muteL.store(channelEntity.muteL);
	channel.muteR.store(channelEntity.muteR);
	channel.soloL.store(channelEntity.soloL);
	channel.soloR.store(channelEntity.soloR);
	channel.panL.store(channelEntity.panL);
	channel.panR.store(channelEntity.panR);

  Logging::write(
  	Info,
  	"Audio::EffectsChannel::ctor",
  	"Instantiating EffectsChannel fromEntity: " + std::to_string(index)
  );
}

EffectsChannel::~EffectsChannel() {
  Logging::write(
  	Info,
  	"Audio::EffectsChannel::dtor",
  	"Destroying EffectsChannel: " + std::to_string(index)
  );

	for (const auto plugin : vst3Plugins)
		delete plugin;

	Logging::write(
  	Info,
  	"Audio::EffectsChannel::dtor",
  	"Destroyed EffectsChannel: " + std::to_string(index)
  );
}

// an effectIdx of -1 indicates to push_back
bool EffectsChannel::addReplaceEffect(const int effectIdx, const std::string& effectPath) {
	Logging::write(
		Info,
		"Audio::EffectsChannel::addReplaceEffect",
		"Adding effect: " + effectPath + " to channel " + std::to_string(index)
	);

	const auto effect = new Vst3::Plugin (
  		effectPath,
  		gAppState,
  		jackClient
  	);

	Logging::write(
		Info,
		"Audio::EffectsChannel::addReplaceEffect",
		"Instantiated plugin " + effect->path + " for channel " + std::to_string(index)
	);

  const FUnknownPtr<IAudioProcessor> processor = effect->getProcesser();
  // int latencySamples = processor->getLatencySamples();
  // incorporateLatencySamples(latencySamples);

  if (!processor->canProcessSampleSize(gAppState->audioFramesPerBuffer)) {
		Logging::write(
			Warning,
			"Audio::EffectsChannel::addReplaceEffect",
			"Processor for " + effectPath + " on channel " + std::to_string(index) + " cannot process sample size " + std::to_string(gAppState->audioFramesPerBuffer)
		);
  	return false;
  }

  // Vst::BusDirection busDirection;
  // int32 index;
  // Vst::SpeakerArrangement arrangement;
  // processor->getBusArrangement(busDirection, index, arrangement);

  const int32 maxSamplesPerBlock = gAppState->audioFramesPerBuffer;
  ProcessSetup setup = {
    kRealtime,
    kSample64,
    maxSamplesPerBlock,
    44100.0
  };
  processor->setupProcessing(setup);
	if (effectIdx < 0) {
		vst3Plugins.push_back(effect);
	} else {
		delete vst3Plugins.at(effectIdx);
		vst3Plugins.at(effectIdx) = effect;
	}

	Logging::write(
		Info,
		"Audio::EffectsChannel::addReplaceEffect",
		"Effect " + effectPath + " added on channel " + std::to_string(index)
	);

	return true;
}

Result EffectsChannel::loadEffect(const Db::Effect& effectEntity) {
	Logging::write(
		Info,
		"Audio::EffectsChannel::loadEffect",
		"Adding effect: " + effectEntity.filePath + " to channel " + std::to_string(index) + " at effectIndex " + std::to_string(effectEntity.effectIndex)
	);

	const auto effect = new Vst3::Plugin(effectEntity, gAppState, jackClient);

	Logging::write(
		Info,
		"Audio::EffectsChannel::loadEffect",
		"Instantiated plugin " + effect->name + " on channel " + std::to_string(index) + " at effectIndex " + std::to_string(effectEntity.effectIndex)
	);

  const FUnknownPtr<IAudioProcessor> processor = effect->getProcesser();
  // int latencySamples = processor->getLatencySamples();
  // incorporateLatencySamples(latencySamples);

  if (!processor->canProcessSampleSize(gAppState->audioFramesPerBuffer)) {
		Logging::write(
			Warning,
			"Audio::EffectsChannel::loadEffect",
			"Processor for " + effectEntity.filePath + " on channel " + std::to_string(index) + " cannot process sample size " + std::to_string(gAppState->audioFramesPerBuffer)
		);
  	return ERROR;
  }

  // Vst::BusDirection busDirection;
  // int32 index;
  // Vst::SpeakerArrangement arrangement;
  // processor->getBusArrangement(busDirection, index, arrangement);

  const int32 maxSamplesPerBlock = gAppState->audioFramesPerBuffer;
  ProcessSetup setup = {
    kRealtime,
    kSample64,
    maxSamplesPerBlock,
    44100.0
  };
  processor->setupProcessing(setup);

	if (vst3Plugins.empty() || effectEntity.effectIndex > vst3Plugins.size() - 1) {
		vst3Plugins.push_back(effect);
	} else {
		delete vst3Plugins.at(effectEntity.effectIndex);
		vst3Plugins.at(effectEntity.effectIndex) = effect;
	}

	Logging::write(
		Info,
		"Audio::EffectsChannel::loadEffect",
		"Effect " + effect->path + " added on channel " + std::to_string(index) + " at effectIndex " + std::to_string(effectEntity.effectIndex)
	);

	return OK;
}


Result EffectsChannel::setSampleRate(const double sampleRate) const {
	bool warning = false;
	for (auto&& plugin : vst3Plugins) {
		if (!plugin->audioHost->audioClient->setSamplerate(sampleRate)) {
			Logging::write(
				Warning,
				"Audio::EffectsChannel::setSampleRate",
				"Unable to set Sample Rate of " + std::to_string(sampleRate) + " for plugin " + plugin->getName() + " channelIndex " + std::to_string(index)
			);
			warning = true;
		}
	}

	return warning ? WARNING : OK;
}

Result EffectsChannel::setBlockSize(const jack_nframes_t blockSize) const {
	bool warning = false;
	const auto blockSize32 = static_cast<int32>(blockSize);
	for (auto&& plugin : vst3Plugins) {
		if (!plugin->audioHost->audioClient->setBlockSize(blockSize32)) {
			Logging::write(
				Warning,
				"Audio::EffectsChannel::setBlockSize",
				"Unable to set Block Size of " + std::to_string(blockSize32) + " + for plugin " + plugin->getName() + " channelIndex " + std::to_string(index)
			);
			warning = true;
		}
	}
	return warning ? WARNING : OK;
}


int EffectsChannel::effectCount() const {
	return static_cast<int>(vst3Plugins.size());
}

void EffectsChannel::initEditorHosts(const std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const {
	Logging::write(
		Info,
		"Audio::EffectsChannel::initEditorHosts",
		"Initializing Editor Hosts on channel " + std::to_string(index)
	);
	int i = 0;
	for (const auto& plugin : vst3Plugins) {
		Logging::write(
			Info,
			"Audio::EffectsChannel::initEditorHosts",
			"Initializing Editor Host for effect " + plugin->path + " on channel " + std::to_string(index)
		);
		plugin->initEditorHost(vstWindows.at(i));
		i++;
	}
}

void EffectsChannel::initEditorHost(const int effectIndex, std::shared_ptr<Gui::VstWindow> vstWindow) const {
		vst3Plugins.at(effectIndex)->initEditorHost(vstWindow);
}

void EffectsChannel::terminateEditorHosts() const {
	Logging::write(
		Info,
		"Audio::EffectsChannel::terminateEditorHosts",
		"Terminating Editor Hosts on channel " + std::to_string(index)
	);

	for (auto&& plugin : vst3Plugins)
		plugin->terminateEditorHost();

	Logging::write(
		Info,
		"Audio::EffectsChannel::terminateEditorHosts",
		"Done terminating Editor Hosts on channel " + std::to_string(index)
	);
}

bool EffectsChannel::removeEffect(const int effectIdx) {
	Logging::write(
		Info,
		"Audio::EffectsChannel::removeEffect",
		"Removing effect " + std::to_string(effectIdx) + " from channel " + std::to_string(index)
	);

	if (effectIdx < vst3Plugins.size()) {
		delete vst3Plugins.at(effectIdx);
		vst3Plugins.erase(vst3Plugins.begin() + effectIdx);
	}
	return true;
}

} // Effects
} // Audio
} // Gj
