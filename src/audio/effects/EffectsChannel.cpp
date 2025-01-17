//
// Created by ns on 1/12/25.
//

#include "EffectsChannel.h"


namespace Gj {
namespace Audio {
namespace Effects {

EffectsChannel::EffectsChannel()
  : channel({ 1.0f, 0.0f })
{}

EffectsChannel::~EffectsChannel() {
  for (auto plugin : vst3Plugins) {
    delete plugin;
  }
  delete this;
}

bool EffectsChannel::addEffect(Vst3::Plugin* plugin) {
  vst3Plugins.push_back(plugin);
  return chainBuffers();
}

bool EffectsChannel::chainBuffers() {
  for (int i = 1; i < vst3Plugins.size(); ++i) {
    auto currentPlugin = vst3Plugins.at(i);
    auto previousPlugin = vst3Plugins.at(i-1);
    auto toFree = currentPlugin->audioHost->buffers.inputs;
    currentPlugin->audioHost->buffers.inputs = previousPlugin->audioHost->buffers.outputs;
    for (int j = 0; j < 2; j++) {
    	free(toFree[j]);
    }
    free(toFree);
  }

  return true;
}

bool EffectsChannel::unchainBuffers() {
  for (int i = 1; i < vst3Plugins.size(); ++i) {
  	vst3Plugins.at(i)->audioHost->allocateInputBuffers();
  }

  return true;
}

} // Effects
} // Audio
} // Gj
