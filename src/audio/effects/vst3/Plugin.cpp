//
// Created by ns on 1/5/25.
//

#include "Plugin.h"

namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

Plugin::Plugin(const std::string& path) :
  name(path),
  path(path) {
    std::string error;

    VST3::Hosting::Module::Ptr module;
	module = VST3::Hosting::Module::create (path, error);
	if (!module)
	{
		std::string reason = "Could not create Module for file:";
		reason += path;
		reason += "\nError: ";
		reason += error;
//		Steinberg::IPlatform::instance ().kill (-1, reason);
	}
    audioHost = new Steinberg::Vst::AudioHost::App;
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
  delete this;
}

bool Plugin::chainBuffers(std::vector<Plugin*>& plugins) {
  // TODO: remove once this responsibility is handed off to EffectsChannel
  for (int i = 1; i < plugins.size(); ++i) {
    auto currentPlugin = plugins.at(i);
    auto previousPlugin = plugins.at(i-1);
    auto toFree = currentPlugin->audioHost->buffers.inputs;
    currentPlugin->audioHost->buffers.inputs = previousPlugin->audioHost->buffers.outputs;
    for (int i = 0; i < 2; i++) {
    	free(toFree[i]);
    }
    free(toFree);
  }

  return true;
}

bool Plugin::unchainBuffers(std::vector<Plugin*>& plugins) {
  // TODO: remove once this responsibility is handed off to EffectsChannel
  for (int i = 1; i < plugins.size(); ++i) {
  	plugins.at(i)->audioHost->allocateInputBuffers();
  }

  return true;
}

} // Vst3
} // Effects
} // Audio
} // Gj
