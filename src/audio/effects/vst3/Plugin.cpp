//
// Created by ns on 1/5/25.
//

#include "Plugin.h"

namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

Plugin::Plugin(const std::string& path, int audioFramesPerBuffer) :
  name(path),
  path(path) {

	std::string error;
	const VST3::Hosting::Module::Ptr module = VST3::Hosting::Module::create(path, error);
	if (!module) {
		std::string reason = "Could not create Module for file:";
		reason += path;
		reason += "\nError: ";
		reason += error;
//		Steinberg::IPlatform::instance ().kill (-1, reason);
	}

	audioHost = new Steinberg::Vst::AudioHost::App(audioFramesPerBuffer);
	audioHost->setModule(module);
	const auto& cmdArgs = std::vector<std::string> { path };
	audioHost->init(cmdArgs);

	editorHost = new Steinberg::Vst::EditorHost::App;
	editorHost->setModule(module);

	editorHost->plugProvider = audioHost->plugProvider;
	editorHost->editController = audioHost->editController;
	editorHost->processorComponent = audioHost->component;

	editorHost->init (cmdArgs);
}

Plugin::~Plugin() {
  audioHost->terminate();
  editorHost->terminate();
}


} // Vst3
} // Effects
} // Audio
} // Gj
