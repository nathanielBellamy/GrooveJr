//
// Created by ns on 1/5/25.
//

#include "Plugin.h"

namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

Plugin::Plugin(std::string path, AppState* gAppState, std::shared_ptr<JackClient> jackClient)
	: gAppState(gAppState)
	, path(path)
	{

	Steinberg::ResizableMemoryIBStream state;

	Logging::write(
		Info,
		"Audio::Plugin::Plugin",
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
			"Audio::Plugin::Plugin",
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
