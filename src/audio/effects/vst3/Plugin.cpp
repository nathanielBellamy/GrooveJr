//
// Created by ns on 1/5/25.
//

#include "Plugin.h"

namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

Plugin::Plugin(const std::string& path, AppState* gAppState, float** inputBuffers, float** outputBuffers, std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows)
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
	const VST3::Hosting::Module::Ptr module = VST3::Hosting::Module::create(path, error);
	if (!module) {
		std::string reason = "Could not create Module for file:";
		reason += path;
		reason += "\nError: ";
		reason += error;
//		Steinberg::IPlatform::instance ().kill (-1, reason);
	}

	const auto& cmdArgs = std::vector<std::string> { path };

	try {
		audioHost = new Steinberg::Vst::AudioHost::App(
			gAppState,
			inputBuffers,
			outputBuffers
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

	try {
		// TODO: cast vstWindows to std::vector<WindowPtr>
		editorHost = new Steinberg::Vst::EditorHost::App(vstWindows);
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

Plugin::~Plugin() {
  audioHost->terminate();
  editorHost->terminate();
}


} // Vst3
} // Effects
} // Audio
} // Gj
