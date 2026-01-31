//
// Created by ns on 1/5/25.
//

#include "Plugin.h"

namespace Gj {
namespace Audio {
namespace Plugins {
namespace Vst3 {
using namespace Steinberg;

Plugin::Plugin(const AtomicStr& path, State::Core* stateCore, std::shared_ptr<JackClient> jackClient)
: stateCore(stateCore)
  , path(path)
  , audioHost(stateCore, jackClient) {
	Steinberg::ResizableMemoryIBStream state;

	Logging::write(
		Info,
		"Audio::Plugin::Plugin::path",
		"Instantiating plugin " + path
	);

	std::string error;
	module = VST3::Hosting::Module::create(path.std_str(), error);
	if (!module) {
		std::string reason = "Could not create Module for file:";
		reason += path.std_str();
		reason += "\nError: ";
		reason += error;
		Logging::write(
			Error,
			"Audio::Plugin::Plugin::path",
			reason
		);
	}

	const auto moduleName = module->getName();
	name = moduleName.substr(0, moduleName.find_last_of('.'));

	const auto& cmdArgs = std::vector{path.std_str()};

	try {
		audioHost.setModule(module);
		audioHost.init(cmdArgs);
	} catch (...) {
		Logging::write(
			Error,
			"Audio::Plugin::Plugin::path",
			"An error occurred while initializing audioHost for " + path
		);
		return;
	}

	Logging::write(
		Info,
		"Audio::Plugin::Plugin::path",
		"Initialized audioHost for " + path
	);
}

Plugin::Plugin(const Db::Plugin& pluginEntity, State::Core* stateCore, std::shared_ptr<JackClient> jackClient)
: stateCore(stateCore)
  , path(pluginEntity.filePath)
  , audioHost(stateCore, jackClient) {
	Logging::write(
		Info,
		"Audio::Plugin::Plugin::entity",
		"Instantiating plugin " + path
	);

	std::string error;
	module = VST3::Hosting::Module::create(path.std_str(), error);
	if (!module) {
		std::string reason = "Could not create Module for file:";
		reason += path.std_str();
		reason += "\nError: ";
		reason += error;
		Logging::write(
			Error,
			"Audio::Plugin::Plugin::entity",
			reason
		);
	}

	const auto moduleName = module->getName();
	name = AtomicStr(moduleName.substr(0, moduleName.find_last_of('.')));

	const auto& cmdArgs = std::vector{path.std_str()};

	try {
		audioHost.setModule(module);
		audioHost.init(cmdArgs);

		const auto audioHostComponentState = owned(new ResizableMemoryIBStream());
		int audioHostComponentStateBytes = pluginEntity.audioHostComponentStateBlob.size();
		int audioHostComponentStateBytesWritten = 0;

		audioHostComponentState->write(
			const_cast<void*>(static_cast<const void*>(pluginEntity.audioHostComponentStateBlob.data())),
			audioHostComponentStateBytes,
			&audioHostComponentStateBytesWritten
		);

		const auto audioHostControllerState = owned(new ResizableMemoryIBStream());
		int audioHostControllerStateBytes = pluginEntity.audioHostControllerStateBlob.size();
		int audioHostControllerStateBytesWritten = 0;

		audioHostControllerState->write(
			const_cast<void*>(static_cast<const void*>(pluginEntity.audioHostControllerStateBlob.data())),
			audioHostControllerStateBytes,
			&audioHostControllerStateBytesWritten
		);

		if (audioHostComponentStateBytesWritten == audioHostComponentStateBytes
		    && audioHostControllerStateBytesWritten == audioHostControllerStateBytes) {
			audioHost.setState(audioHostComponentState.get(), audioHostControllerState.get());
		} else {
			Logging::write(
				Error,
				"Audio::Plugin::Plugin::entity",
				"Unable to prepare audioHostComponentState"
			);
		}

		editorHostComponentStateStream = owned(new ResizableMemoryIBStream());
		int editorHostComponentStateBytes = pluginEntity.audioHostComponentStateBlob.size();
		int editorHostComponentStateBytesWritten = 0;

		editorHostComponentStateStream->write(
			const_cast<void*>(static_cast<const void*>(pluginEntity.editorHostComponentStateBlob.data())),
			editorHostComponentStateBytes,
			&editorHostComponentStateBytesWritten
		);

		editorHostControllerStateStream = owned(new ResizableMemoryIBStream());
		int editorHostControllerStateBytes = pluginEntity.audioHostControllerStateBlob.size();
		int editorHostControllerStateBytesWritten = 0;

		editorHostControllerStateStream->write(
			const_cast<void*>(static_cast<const void*>(pluginEntity.editorHostControllerStateBlob.data())),
			editorHostControllerStateBytes,
			&editorHostControllerStateBytesWritten
		);
	} catch (...) {
		Logging::write(
			Error,
			"Audio::Plugin::Plugin",
			"An error occurred while initializing audioHost for " + path
		);
		return;
	}

	Logging::write(
		Info,
		"Audio::Plugin::Plugin",
		"Initialized audioHost for " + path
	);
}

void Plugin::initEditorHost(EditorHost::WindowPtr window) {
	try {
		Logging::write(
			Info,
			"Audio::Plugin::initEditorHost",
			"Initializing editorHost for " + this->path
		);
		const auto& cmdArgs = std::vector{path.std_str()};
		editorHost = std::make_unique<EditorHost::App>(window);
		editorHost->setModule(module);
		editorHost->plugProvider = audioHost.plugProvider;
		editorHost->editController = audioHost.editController;
		editorHost->processorComponent = audioHost.component;
		editorHost->init(cmdArgs);

		editorHost->setState(editorHostComponentStateStream.get(), editorHostComponentStateStream.get());
	} catch (...) {
		Logging::write(
			Warning,
			"Audio::Plugin::Plugin",
			"An error occurred while initiating editorHost for " + path
		);
		return;
	}

	Logging::write(
		Info,
		"Audio::Plugin::Plugin",
		"Initialized editorHost for " + path
	);
}

Result Plugin::cacheEditorHostState() const {
	if (editorHost == nullptr) {
		Logging::write(
			Error,
			"Audio::Plugin::cacheEditorHostState",
			"Attempting to retrieve the editorHost state from an empty editorHost"
		);
		return ERROR;
	}

	editorHost->getState(editorHostComponentStateStream.get(), editorHostControllerStateStream.get());

	return OK;
}


Result Plugin::populateEditorHostStateBuffers(std::vector<uint8_t>& componentStateBuffer,
                                              std::vector<uint8_t>& controllerStateBuffer) const {
	if (editorHost == nullptr) {
		Logging::write(
			Error,
			"Audio::Plugin::getEditorHostState",
			"Attempting to retrieve the editorHost state from an empty editorHost"
		);
		return ERROR;
	}

	editorHost->getState(editorHostComponentStateStream.get(), editorHostControllerStateStream.get());

	int64 editorHostComponentStateSize = 0;
	if (Util::getStreamSize(editorHostComponentStateStream.get(), &editorHostComponentStateSize) != OK) {
		Logging::write(
			Error,
			"Audio::Plugins::Vst3::Plugin::populateEditorHostState",
			"Unable to determine stream size for editorHostComponentStateStream"
		);
		return ERROR;
	}

	int64 editorHostControllerStateSize = 0;
	if (Util::getStreamSize(editorHostControllerStateStream.get(), &editorHostControllerStateSize) != OK) {
		Logging::write(
			Error,
			"Audio::Plugins::Vst3::Plugin::populateEditorHostState",
			"Unable to determine stream size for editorHostControllerStateStream"
		);
		return ERROR;
	}

	componentStateBuffer.resize(editorHostComponentStateSize);
	controllerStateBuffer.resize(editorHostControllerStateSize);

	int32 editorHostComponentNumBytesRead = 0;
	int32 editorHostControllerNumBytesRead = 0;

	if (const auto componentRes = editorHostComponentStateStream->read(
		componentStateBuffer.data(),
		static_cast<int32>(editorHostComponentStateSize),
		&editorHostComponentNumBytesRead
	); componentRes != kResultOk) {
		Logging::write(
			Error,
			"Audio::Plugins::Vst3::Plugin::populateEditorHostState",
			"Unable to read editorHostComponentStateStream. tresult: " + std::to_string(componentRes)
		);
		return ERROR;
	}

	if (const auto controllerRes = editorHostControllerStateStream->read(
		controllerStateBuffer.data(),
		static_cast<int32>(editorHostControllerStateSize),
		&editorHostControllerNumBytesRead
	); controllerRes != kResultOk) {
		Logging::write(
			Error,
			"Audio::Plugins::Vst3::Plugin::populateEditorHostState",
			"Unable to read editorHostControllerStateStream. tresult: " + std::to_string(controllerRes)
		);
		return ERROR;
	};

	return OK;
}


Result Plugin::terminateEditorHost() {
	Logging::write(
		Info,
		"Audio::Plugins::Vst3::Plugin::terminateEditorHost",
		"Terminating editorHost for Plugin: " + this->name
	);

	if (editorHost == nullptr) {
		Logging::write(
			Warning,
			"Audio::Plugins::Vst3::Plugin::terminateEditorHost",
			"Attempting to terminate null editorHost."
		);
		return WARNING;
	}

	if (const auto cacheRes = cacheEditorHostState(); cacheRes != OK) {
		// TODO: update editorHost->getState to return a Result
		Logging::write(
			Error,
			"Audio::Plugins::Vst3::Plugin::terminateEditorHost",
			"Unable to cache editorHost State. Result: " + std::to_string(cacheRes)
		);
	};

	editorHost->terminate();
	editorHost.reset();

	Logging::write(
		Info,
		"Audio::Plugins::Vst3::Plugin::terminateEditorHost",
		"Done terminating editorHost for Plugin: " + this->name
	);
	return OK;
}
} // Vst3
} // Plugins
} // Audio
} // Gj
