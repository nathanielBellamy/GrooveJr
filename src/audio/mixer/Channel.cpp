//
// Created by ns on 1/12/25.
//

#include "Channel.h"

#include <caf/log/core.hpp>
#include <caf/log/level.hpp>


namespace Gj {
namespace Audio {
namespace Mixer {
using namespace Steinberg;

Channel::Channel(
	AppState* gAppState,
	std::shared_ptr<JackClient> jackClient,
	const ChannelIndex index
)
: gAppState(gAppState)
  , jackClient(jackClient)
  , index(index) {
	Logging::write(
		Info,
		"Audio::Mixer::Channel::ctor",
		"Instantiating MixerChannel: " + std::to_string(index)
	);
}

Channel::Channel(
	AppState* gAppState,
	std::shared_ptr<JackClient> jackClient,
	const Db::ChannelEntity& channelEntity
)
: gAppState(gAppState)
  , jackClient(jackClient)
  , index(channelEntity.channelIndex)
  , name(channelEntity.name) {
	settings.gain.store(channelEntity.gain);
	settings.mute.store(channelEntity.mute);
	settings.solo.store(channelEntity.solo);
	settings.pan.store(channelEntity.pan);
	settings.gainL.store(channelEntity.gainL);
	settings.gainR.store(channelEntity.gainR);
	settings.muteL.store(channelEntity.muteL);
	settings.muteR.store(channelEntity.muteR);
	settings.soloL.store(channelEntity.soloL);
	settings.soloR.store(channelEntity.soloR);
	settings.panL.store(channelEntity.panL);
	settings.panR.store(channelEntity.panR);

	Logging::write(
		Info,
		"Audio::Mixer::Channel::ctor",
		"Instantiating MixerChannel fromEntity: " + std::to_string(index)
	);
}

Channel::~Channel() {
	Logging::write(
		Info,
		"Audio::Mixer::Channel::dtor",
		"Destroying MixerChannel: " + std::to_string(index)
	);

	if (forEachPlugin([this](const Plugins::Vst3::Plugin* plugin, const PluginIndex) {
		delete plugin;
	}) != OK)
		Logging::write(
			Warning,
			"Audio::Mixer::Channel::dtor",
			"An Warning or Error occurred while destroying a plugin on ChannelIndex: " + std::to_string(index)
		);

	Logging::write(
		Info,
		"Audio::Mixer::Channel::dtor",
		"Destroyed Channel: " + std::to_string(index)
	);
}

std::optional<PluginIndex> Channel::firstOpenPluginIndex() const {
	for (PluginIndex i = 0; i < MAX_PLUGINS_PER_CHANNEL; ++i) {
		if (!plugins[i])
			return std::optional(i);
	}
	return std::nullopt;
}

Result Channel::addReplacePlugin(const std::optional<PluginIndex> pluginIdxOpt, const PluginPath& pluginPath) {
	Logging::write(
		Info,
		"Audio::Mixer::Channel::addReplacePlugin",
		"Adding plugin: " + pluginPath + " to channel " + std::to_string(index)
	);

	const auto plugin = new Plugins::Vst3::Plugin(
		pluginPath,
		gAppState,
		jackClient
	);

	Logging::write(
		Info,
		"Audio::Mixer::Channel::addReplacePlugin",
		"Instantiated plugin " + plugin->path + " for channel " + std::to_string(index)
	);

	const FUnknownPtr<IAudioProcessor> processor = plugin->getProcesser();
	// int latencySamples = processor->getLatencySamples();
	// incorporateLatencySamples(latencySamples);

	const auto gAppStateAudioFramesPerBuffer = static_cast<int32>(gAppState->getAudioFramesPerBuffer());
	if (!processor->canProcessSampleSize(gAppStateAudioFramesPerBuffer)) {
		Logging::write(
			Warning,
			"Audio::Mixer::Channel::addReplacePlugin",
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
	if (!pluginIdxOpt) {
		const auto idxToPut = firstOpenPluginIndex();
		if (!idxToPut) {
			Logging::write(
				Warning,
				"Audio::Mixer::Channel::addReplacePlugin",
				"Attempted to add too many plugins to Channel: " + std::to_string(index)
			);
			return WARNING;
		}
		plugins[idxToPut.value()] = plugin;
	} else {
		delete plugins[pluginIdxOpt.value()].value_or(nullptr);
		plugins[pluginIdxOpt.value()] = plugin;
	}

	Logging::write(
		Info,
		"Audio::MixerChannel::addReplacePlugin",
		"Plugin " + pluginPath + " added on channel " + std::to_string(index)
	);

	return OK;
}

Result Channel::loadPlugin(const Db::Plugin& pluginEntity) {
	Logging::write(
		Info,
		"Audio::MixerChannel::loadPlugin",
		"Adding plugin: " + pluginEntity.filePath + " to channel " + std::to_string(index) + " at pluginIndex " +
		std::to_string(pluginEntity.pluginIndex)
	);

	const auto plugin = new Plugins::Vst3::Plugin(pluginEntity, gAppState, jackClient);

	Logging::write(
		Info,
		"Audio::Mixer::Channel::loadPlugin",
		"Instantiated plugin " + plugin->name + " on channel " + std::to_string(index) + " at pluginIndex " +
		std::to_string(pluginEntity.pluginIndex)
	);

	const FUnknownPtr<IAudioProcessor> processor = plugin->getProcesser();
	// int latencySamples = processor->getLatencySamples();
	// incorporateLatencySamples(latencySamples);

	if (!processor->canProcessSampleSize(gAppState->getAudioFramesPerBuffer())) {
		Logging::write(
			Warning,
			"Audio::Mixer::Channel::loadPlugin",
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
		"Audio::Mixer::Channel::loadPlugin",
		"Plugin " + plugin->path + " added on channel " + std::to_string(index) + " at pluginIndex " + std::to_string(
			pluginEntity.pluginIndex)
	);

	return OK;
}


Result Channel::setSampleRate(const double sampleRate) {
	bool warning = false;
	const Result setResult = forEachPlugin(
		[this, &sampleRate, &warning](Plugins::Vst3::Plugin* plugin, PluginIndex pluginIndex) {
			if (!plugin->audioHost->audioClient->setSamplerate(sampleRate)) {
				Logging::write(
					Warning,
					"Audio::Mixer::Channel::setSampleRate",
					"Unable to set Sample Rate of " + std::to_string(sampleRate) + " for plugin " + plugin->getName() +
					" channelIndex " + std::to_string(index)
				);
				warning = true;
			}
		});

	return warning || setResult != OK
		       ? WARNING
		       : OK;
}

Result Channel::setBlockSize(const jack_nframes_t blockSize) {
	bool warning = false;
	const auto blockSize32 = static_cast<int32>(blockSize);
	const auto setResult = forEachPlugin(
		[this, &blockSize32, &warning](Plugins::Vst3::Plugin* plugin, PluginIndex pluginIndex) {
			if (!plugin->audioHost->audioClient->setBlockSize(blockSize32)) {
				Logging::write(
					Warning,
					"Audio::Mixer::Channel::setBlockSize",
					"Unable to set Block Size of " + std::to_string(blockSize32) + " + for plugin " + plugin->getName() +
					" channelIndex " + std::to_string(index)
				);
				warning = true;
			}
		});
	return warning || setResult != OK
		       ? WARNING
		       : OK;
}


PluginIndex Channel::pluginCount() const {
	PluginIndex res = 0;

	std::for_each(std::begin(plugins), std::end(plugins), [&res](std::optional<Plugins::Vst3::Plugin*> plugin) {
		if (plugin)
			res++;
	});

	return res;
}

Result Channel::initEditorHosts(const std::vector<std::shared_ptr<Gui::Mixer::VstWindow> >& vstWindows) {
	Logging::write(
		Info,
		"Audio::Mixer::Channel::initEditorHosts",
		"Initializing Editor Hosts on channel " + std::to_string(index)
	);
	return forEachPlugin([this, &vstWindows](Plugins::Vst3::Plugin* plugin, const PluginIndex pluginIndex) {
		plugin->initEditorHost(vstWindows.at(pluginIndex));
	});
}

Result Channel::initEditorHost(const PluginIndex pluginIndex, std::shared_ptr<Gui::Mixer::VstWindow> vstWindow) const {
	if (const auto pluginOpt = getPluginAtIdx(pluginIndex); pluginOpt.has_value())
		pluginOpt.value()->initEditorHost(vstWindow);

	return OK;
}

Result Channel::terminateEditorHosts() {
	Logging::write(
		Info,
		"Audio::Mixer::Channel::terminateEditorHosts",
		"Terminating Editor Hosts on channel " + std::to_string(index)
	);

	if (forEachPlugin([this](const Plugins::Vst3::Plugin* plugin, const PluginIndex) {
		plugin->terminateEditorHost();
	}) != OK) {
		Logging::write(
			Warning,
			"Audio::Mixer::Channel::terminateEditorHosts",
			"An Error or Warning occurred while terminating EditorHost on ChannelIndex: " + std::to_string(index)
		);
		return WARNING;
	}

	Logging::write(
		Info,
		"Audio::Mixer::Channel::terminateEditorHosts",
		"Done terminating Editor Hosts on channel " + std::to_string(index)
	);
	return OK;
}

Result Channel::removePlugin(const PluginIndex pluginIdx) {
	Logging::write(
		Info,
		"Audio::Mixer::Channel::removePlugin",
		"Removing plugin " + std::to_string(pluginIdx) + " from channel " + std::to_string(index)
	);

	if (pluginIdx > MAX_PLUGINS_PER_CHANNEL) {
		Logging::write(
			Error,
			"Audio::Mixer::Channel::removePlugin",
			"Attempting to remove plugin at out of range idx: " + std::to_string(pluginIdx) + ". MAX_PLUGINS_PER_CHANNEL: "
			+
			std::to_string(MAX_PLUGINS_PER_CHANNEL)
		);
		return ERROR;
	}

	delete plugins[pluginIdx].value_or(nullptr);
	plugins[pluginIdx].reset();

	return OK;
}
} // Mixer
} // Audio
} // Gj
