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
	const ChannelIndex index
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

Result EffectsChannel::addReplacePlugin(const std::optional<PluginIndex> effectIdxOpt, const PluginPath& pluginPath) {
	Logging::write(
		Info,
		"Audio::EffectsChannel::addReplacePlugin",
		"Adding effect: " + pluginPath + " to channel " + std::to_string(index)
	);

	const auto plugin = new Vst3::Plugin (
  		pluginPath,
  		gAppState,
  		jackClient
  	);

	Logging::write(
		Info,
		"Audio::EffectsChannel::addReplacePlugin",
		"Instantiated plugin " + plugin->path + " for channel " + std::to_string(index)
	);

  const FUnknownPtr<IAudioProcessor> processor = plugin->getProcesser();
  // int latencySamples = processor->getLatencySamples();
  // incorporateLatencySamples(latencySamples);

	const auto gAppStateAudioFramesPerBuffer = static_cast<int32>(gAppState->getAudioFramesPerBuffer());
  if (!processor->canProcessSampleSize(gAppStateAudioFramesPerBuffer)) {
		Logging::write(
			Warning,
			"Audio::EffectsChannel::addReplacePlugin",
			"Processor for " + pluginPath + " on channel " + std::to_string(index) + " cannot process sample size " + std::to_string(gAppState->audioFramesPerBuffer)
		);
  	return WARNING;
  }

  // Vst::BusDirection busDirection;
  // int32 index;
  // Vst::SpeakerArrangement arrangement;
  // processor->getBusArrangement(busDirection, index, arrangement);

  ProcessSetup setup = {
    kRealtime,
    kSample64,
    AUDIO_FRAMES_PER_BUFFER_MAX,
    44100.0
  };
  processor->setupProcessing(setup);
	if (!effectIdxOpt) {
		vst3Plugins.push_back(plugin);
	} else {
		delete vst3Plugins.at(effectIdxOpt.value());
		vst3Plugins.at(effectIdxOpt.value()) = plugin;
	}

	Logging::write(
		Info,
		"Audio::EffectsChannel::addReplacePlugin",
		"Plugin " + pluginPath + " added on channel " + std::to_string(index)
	);

	return OK;
}

Result EffectsChannel::loadPlugin(const Db::Plugin& pluginEntity) {
	Logging::write(
		Info,
		"Audio::EffectsChannel::loadPlugin",
		"Adding effect: " + pluginEntity.filePath + " to channel " + std::to_string(index) + " at effectIndex " + std::to_string(pluginEntity.pluginIndex)
	);

	const auto effect = new Vst3::Plugin(pluginEntity, gAppState, jackClient);

	Logging::write(
		Info,
		"Audio::EffectsChannel::loadPlugin",
		"Instantiated plugin " + effect->name + " on channel " + std::to_string(index) + " at effectIndex " + std::to_string(pluginEntity.pluginIndex)
	);

  const FUnknownPtr<IAudioProcessor> processor = effect->getProcesser();
  // int latencySamples = processor->getLatencySamples();
  // incorporateLatencySamples(latencySamples);

  if (!processor->canProcessSampleSize(gAppState->getAudioFramesPerBuffer())) {
		Logging::write(
			Warning,
			"Audio::EffectsChannel::loadPlugin",
			"Processor for " + pluginEntity.filePath + " on channel " + std::to_string(index) + " cannot process sample size " + std::to_string(gAppState->audioFramesPerBuffer)
		);
  	return ERROR;
  }

  // Vst::BusDirection busDirection;
  // int32 index;
  // Vst::SpeakerArrangement arrangement;
  // processor->getBusArrangement(busDirection, index, arrangement);

  ProcessSetup setup = {
    kRealtime,
    kSample64,
    AUDIO_FRAMES_PER_BUFFER_MAX,
    44100.0
  };
  processor->setupProcessing(setup);

	if (vst3Plugins.empty() || pluginEntity.pluginIndex > vst3Plugins.size() - 1) {
		vst3Plugins.push_back(effect);
	} else {
		delete vst3Plugins.at(pluginEntity.pluginIndex);
		vst3Plugins.at(pluginEntity.pluginIndex) = effect;
	}

	Logging::write(
		Info,
		"Audio::EffectsChannel::loadPlugin",
		"Effect " + effect->path + " added on channel " + std::to_string(index) + " at effectIndex " + std::to_string(pluginEntity.pluginIndex)
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


PluginIndex EffectsChannel::pluginCount() const {
	return vst3Plugins.size();
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

void EffectsChannel::initEditorHost(const PluginIndex pluginIndex, std::shared_ptr<Gui::VstWindow> vstWindow) const {
		vst3Plugins.at(pluginIndex)->initEditorHost(vstWindow);
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

Result EffectsChannel::removePlugin(const PluginIndex pluginIdx) {
	Logging::write(
		Info,
		"Audio::EffectsChannel::removePlugin",
		"Removing plugin " + std::to_string(pluginIdx) + " from channel " + std::to_string(index)
	);

	if (pluginIdx < vst3Plugins.size()) {
		delete vst3Plugins.at(pluginIdx);
		vst3Plugins.erase(vst3Plugins.begin() + pluginIdx);
	}

	return OK;
}

} // Effects
} // Audio
} // Gj
