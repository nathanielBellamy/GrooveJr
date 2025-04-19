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
	, name(path)
	, path(path)
	{

	Logging::write(
		Info,
		"Plugin::Plugin()",
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
			LogSeverityLevel::Error,
			"Plugin::Plugin()",
			reason
		);
	}

	const auto& cmdArgs = std::vector<std::string> { path };

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
			"Plugin::Plugin()",
			"An error occurred while initialiazing audioHost for " + path
		);
		return;
	}

	Logging::write(
		Info,
		"Plugin::Plugin()",
		"Initialized audioHost for " + path
	);
}

Plugin::~Plugin() {
  audioHost->terminate();
  editorHost->terminate();
}

void Plugin::initEditorHost(EditorHost::WindowPtr window) {
	try {
		Logging::write(
			Info,
			"Plugin::initEditorHost()",
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
			"Plugin::Plugin()",
			"An error occured while initiating editorhost for " + path
		);
		return;
	}

	Logging::write(
		Info,
		"Plugin::Plugin()",
		"Initialized editorHost for " + path
	);
}

void Plugin::terminateEditorHost() const {
	editorHost->terminate();
}

} // Vst3
} // Effects
} // Audio
} // Gj
