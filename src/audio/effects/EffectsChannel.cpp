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
  , index(index) {
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
  , name(channelEntity.name) {
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

	for (const auto plugin: plugins)
		delete plugin.value_or(nullptr);

	Logging::write(
		Info,
		"Audio::EffectsChannel::dtor",
		"Destroyed EffectsChannel: " + std::to_string(index)
	);
}

std::optional<PluginIndex> EffectsChannel::firstOpenPluginIndex() const {
	for (PluginIndex i = 0; i < MAX_PLUGINS_PER_CHANNEL; i++) {
		if (!plugins[i].has_value())
			return std::optional(i);
	}
	return std::nullopt;
}


Result EffectsChannel::addReplacePlugin(const std::optional<PluginIndex> effectIdxOpt, const PluginPath& pluginPath) {
	Logging::write(
		Info,
		"Audio::EffectsChannel::addReplacePlugin",
		"Adding effect: " + pluginPath + " to channel " + std::to_string(index)
	);

	const auto plugin = new Vst3::Plugin(
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
			"Processor for " + pluginPath + " on channel " + std::to_string(index) + " cannot process sample size " +
			std::to_string(gAppState->audioFramesPerBuffer)
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
		const auto idxToPut = firstOpenPluginIndex();
		if (!idxToPut) {
			Logging::write(
				Warning,
				"Audio::EffectsChannel::addReplacePlugin",
				"Attempted to add too many plugins to Channel: " + std::to_string(index)
			);
			return WARNING;
		}
		plugins[idxToPut.value()] = plugin;
	} else {
		delete plugins[effectIdxOpt.value()].value_or(nullptr);
		plugins[effectIdxOpt.value()] = plugin;
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
		"Adding effect: " + pluginEntity.filePath + " to channel " + std::to_string(index) + " at effectIndex " +
		std::to_string(pluginEntity.pluginIndex)
	);

	const auto plugin = new Vst3::Plugin(pluginEntity, gAppState, jackClient);

	Logging::write(
		Info,
		"Audio::EffectsChannel::loadPlugin",
		"Instantiated plugin " + plugin->name + " on channel " + std::to_string(index) + " at effectIndex " +
		std::to_string(pluginEntity.pluginIndex)
	);

	const FUnknownPtr<IAudioProcessor> processor = plugin->getProcesser();
	// int latencySamples = processor->getLatencySamples();
	// incorporateLatencySamples(latencySamples);

	if (!processor->canProcessSampleSize(gAppState->getAudioFramesPerBuffer())) {
		Logging::write(
			Warning,
			"Audio::EffectsChannel::loadPlugin",
			"Processor for " + pluginEntity.filePath + " on channel " + std::to_string(index) + " cannot process sample size "
			+ std::to_string(gAppState->audioFramesPerBuffer)
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

	delete plugins[pluginEntity.pluginIndex].value_or(nullptr);
	plugins[pluginEntity.pluginIndex] = std::optional(plugin);

	Logging::write(
		Info,
		"Audio::EffectsChannel::loadPlugin",
		"Effect " + plugin->path + " added on channel " + std::to_string(index) + " at effectIndex " + std::to_string(
			pluginEntity.pluginIndex)
	);

	return OK;
}


Result EffectsChannel::setSampleRate(const double sampleRate) const {
	bool warning = false;
	for (auto&& plugin: plugins) {
		if (!plugin) continue;
		if (!plugin.value()->audioHost->audioClient->setSamplerate(sampleRate)) {
			Logging::write(
				Warning,
				"Audio::EffectsChannel::setSampleRate",
				"Unable to set Sample Rate of " + std::to_string(sampleRate) + " for plugin " + plugin.value()->getName() +
				" channelIndex " + std::to_string(index)
			);
			warning = true;
		}
	}

	return warning ? WARNING : OK;
}

Result EffectsChannel::setBlockSize(const jack_nframes_t blockSize) const {
	bool warning = false;
	const auto blockSize32 = static_cast<int32>(blockSize);
	for (auto&& plugin: plugins) {
		if (!plugin) continue;
		if (!plugin.value()->audioHost->audioClient->setBlockSize(blockSize32)) {
			Logging::write(
				Warning,
				"Audio::EffectsChannel::setBlockSize",
				"Unable to set Block Size of " + std::to_string(blockSize32) + " + for plugin " + plugin.value()->getName() +
				" channelIndex " + std::to_string(index)
			);
			warning = true;
		}
	}
	return warning ? WARNING : OK;
}


PluginIndex EffectsChannel::pluginCount() const {
	PluginIndex res = 0;

	std::for_each(std::begin(plugins), std::end(plugins), [&res](std::optional<Vst3::Plugin*> plugin) {
		if (plugin)
			res++;
	});

	return res;
}

void EffectsChannel::initEditorHosts(const std::vector<std::shared_ptr<Gui::VstWindow> >& vstWindows) const {
	Logging::write(
		Info,
		"Audio::EffectsChannel::initEditorHosts",
		"Initializing Editor Hosts on channel " + std::to_string(index)
	);
	int i = 0;
	for (const auto& plugin: plugins) {
		if (!plugin) continue;
		Logging::write(
			Info,
			"Audio::EffectsChannel::initEditorHosts",
			"Initializing Editor Host for effect " + plugin.value()->path + " on channel " + std::to_string(index)
		);
		plugin.value()->initEditorHost(vstWindows.at(i));
		i++;
	}
}

void EffectsChannel::initEditorHost(const PluginIndex pluginIndex, std::shared_ptr<Gui::VstWindow> vstWindow) const {
	if (const auto pluginOpt = getPluginAtIdx(pluginIndex); pluginOpt.has_value())
		pluginOpt.value()->initEditorHost(vstWindow);
}

void EffectsChannel::terminateEditorHosts() const {
	Logging::write(
		Info,
		"Audio::EffectsChannel::terminateEditorHosts",
		"Terminating Editor Hosts on channel " + std::to_string(index)
	);

	for (auto&& plugin: plugins) {
		if (!plugin) continue;
		plugin.value()->terminateEditorHost();
	}

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

	if (pluginIdx > MAX_PLUGINS_PER_CHANNEL) {
		Logging::write(
			Error,
			"Audio::EffectsChannel::removePlugin",
			"Attempting to remove plugin at out of range idx: " + std::to_string(pluginIdx) + ". MAX_PLUGINS_PER_CHANNEL: " +
			std::to_string(MAX_PLUGINS_PER_CHANNEL)
		);
		return ERROR;
	}

	delete plugins[pluginIdx].value_or(nullptr);
	plugins[pluginIdx].reset();

	return OK;
}
} // Effects
} // Audio
} // Gj
