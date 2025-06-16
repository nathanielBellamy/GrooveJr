//
// Created by ns on 1/5/25.
//

#include "Plugin.h"

namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

using namespace Steinberg;

Plugin::Plugin(std::string path, AppState* gAppState, std::shared_ptr<JackClient> jackClient)
	: gAppState(gAppState)
	, path(path)
	{

	Steinberg::ResizableMemoryIBStream state;

	Logging::write(
		Info,
		"Audio::Plugin::Plugin::path",
		"Instantiating plugin " + path
	);

	std::string error;
	module = VST3::Hosting::Module::create(path, error);
	if (!module) {
		std::string reason = "Could not create Module for file:";
		reason += path;
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

	const auto& cmdArgs = std::vector { path };

	try {
		audioHost = new AudioHost::App(
			gAppState,
			jackClient
		);
		audioHost->setModule(module);
		audioHost->init(cmdArgs);
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

Plugin::Plugin(const Db::Effect& effectEntity, AppState* gAppState, std::shared_ptr<JackClient> jackClient)
	: gAppState(gAppState)
	, path(effectEntity.filePath)
	{

	Logging::write(
		Info,
		"Audio::Plugin::Plugin::entity",
		"Instantiating plugin " + path
	);

	std::string error;
	module = VST3::Hosting::Module::create(path, error);
	if (!module) {
		std::string reason = "Could not create Module for file:";
		reason += path;
		reason += "\nError: ";
		reason += error;
		Logging::write(
			Error,
			"Audio::Plugin::Plugin::entity",
			reason
		);
	}

	const auto moduleName = module->getName();
	name = moduleName.substr(0, moduleName.find_last_of('.'));

	const auto& cmdArgs = std::vector { path };

	try {
		audioHost = new AudioHost::App(
			gAppState,
			jackClient
		);
		audioHost->setModule(module);
		audioHost->init(cmdArgs);

		const auto audioHostComponentState = std::make_unique<Steinberg::ResizableMemoryIBStream>();
		int audioHostComponentStateBytes = effectEntity.audioHostComponentStateBlob.size();
		int audioHostComponentStateBytesWritten = 0;

		audioHostComponentState->write(
			const_cast<void*>(static_cast<const void*>(effectEntity.audioHostComponentStateBlob.data())),
			audioHostComponentStateBytes,
			&audioHostComponentStateBytesWritten
		);

		const auto audioHostControllerState = std::make_unique<Steinberg::ResizableMemoryIBStream>();
		int audioHostControllerStateBytes = effectEntity.audioHostControllerStateBlob.size();
		int audioHostControllerStateBytesWritten = 0;

		audioHostControllerState->write(
			const_cast<void*>(static_cast<const void*>(effectEntity.audioHostControllerStateBlob.data())),
			audioHostControllerStateBytes,
			&audioHostControllerStateBytesWritten
		);

		if (audioHostComponentStateBytesWritten == audioHostComponentStateBytes
					&& audioHostControllerStateBytesWritten == audioHostControllerStateBytes) {
			audioHost->setState(audioHostComponentState.get(), audioHostControllerState.get());
		} else {
			Logging::write(
				Error,
				"Audio::Plugin::Plugin::entity",
				"Unable to prepare audioHostComponentState"
			);
		}

		editorHostComponentStateStream = std::make_unique<Steinberg::ResizableMemoryIBStream>();
		int editorHostComponentStateBytes = effectEntity.audioHostComponentStateBlob.size();
		int editorHostComponentStateBytesWritten = 0;

		editorHostComponentStateStream->write(
			const_cast<void*>(static_cast<const void*>(effectEntity.editorHostComponentStateBlob.data())),
			editorHostComponentStateBytes,
			&editorHostComponentStateBytesWritten
		);

		editorHostControllerStateStream = std::make_unique<Steinberg::ResizableMemoryIBStream>();
		int editorHostControllerStateBytes = effectEntity.audioHostControllerStateBlob.size();
		int editorHostControllerStateBytesWritten = 0;

		editorHostControllerStateStream->write(
			const_cast<void*>(static_cast<const void*>(effectEntity.editorHostControllerStateBlob.data())),
			editorHostControllerStateBytes,
			&editorHostControllerStateBytesWritten
		);
	} catch (...) {
		Logging::write(
			Error,
			"Audio::Plugin::Plugin",
			"An error occurred while initialiazing audioHost for " + path
		);
		return;
	}

	Logging::write(
		Info,
		"Audio::Plugin::Plugin",
		"Initialized audioHost for " + path
	);
}

Plugin::~Plugin() {
	Logging::write(
		Info
		, "Audio::Plugin::~Plugin",
		"Destroying Plugin: " + name
	);
  audioHost->terminate();

	Logging::write(
		Info,
		"Audio::Plugin::~Plugin",
		"Destroyed audioHost for Plugin: " + name
	);

	if (editorHost != nullptr) {
		delete editorHost;

		Logging::write(
			Info
			, "Audio::Plugin::~Plugin",
			"Destroyed editorHost for Plugin: " + name
		);
	}

	Logging::write(
		Info,
		"Audio::Plugin::~Plugin",
		"Destroyed Plugin: " + name
	);
}

void Plugin::initEditorHost(EditorHost::WindowPtr window) {
	try {
		Logging::write(
			Info,
			"Audio::Plugin::initEditorHost",
			"Initializing editorHost for " + this->path
		);
		const auto& cmdArgs = std::vector { path };
		editorHost = new EditorHost::App(window);
		editorHost->setModule(module);
		editorHost->plugProvider = audioHost->plugProvider;
		editorHost->editController = audioHost->editController;
		editorHost->processorComponent = audioHost->component;
		editorHost->init (cmdArgs);

		editorHost->setState(editorHostComponentStateStream.get(), editorHostComponentStateStream.get());
	} catch (...) {
		Logging::write(
			Warning,
			"Audio::Plugin::Plugin",
			"An error occured while initiating editorhost for " + path
		);
		return;
	}

	Logging::write(
		Info,
		"Audio::Plugin::Plugin",
		"Initialized editorHost for " + path
	);
}

Result Plugin::peristEditorHostState() {
		int64 editorHostComponentStateSize = 0;
		if (Util::getStreamSize(editorHostComponentStateStream.get(), &editorHostComponentStateSize) != OK) {
			Logging::write(
				Error,
				"Audio::Mixer::saveScene",
				"Unable to determine stream size for editorHostComponentStateStream"
			);
		}

		int64 editorHostControllerStateSize = 0;
		if (Util::getStreamSize(editorHostControllerStateStream.get(), &editorHostControllerStateSize) != OK) {
			Logging::write(
				Error,
				"Audio::Mixer::saveScene",
				"Unable to determine stream size for editorHostControllerStateStream"
			);
		}

	  std::vector<uint8_t> editorHostComponentBuffer (editorHostComponentStateSize);
		std::vector<uint8_t> editorHostControllerBuffer (editorHostControllerStateSize);

		int32 editorHostComponentNumBytesRead = 0;
		int32 editorHostControllerNumBytesRead = 0;

		editorHostComponentStateStream->read(
			editorHostComponentBuffer.data(),
			static_cast<int32>(editorHostComponentStateSize),
			&editorHostComponentNumBytesRead
		);

		editorHostControllerStateStream->read(
			editorHostControllerBuffer.data(),
			static_cast<int32>(editorHostControllerStateSize),
			&editorHostControllerNumBytesRead
		);

	return OK;
}


void Plugin::terminateEditorHost() const {
	Logging::write(
		Info,
		"Audio::Plugin::terminateEditorHost",
		"Terminating editorHost for Plugin: " + this->name
	);

	if (editorHost != nullptr)
		editorHost->terminate();

	Logging::write(
		Info,
		"Audio::Plugin::terminateEditorHost",
		"Done terminating editorHost for Plugin: " + this->name
	);
}

} // Vst3
} // Effects
} // Audio
} // Gj
