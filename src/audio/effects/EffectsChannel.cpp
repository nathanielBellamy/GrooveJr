//
// Created by ns on 1/12/25.
//

#include "EffectsChannel.h"


namespace Gj {
namespace Audio {
namespace Effects {

EffectsChannel::EffectsChannel(float** inputBuffers)
  : inputBuffers(inputBuffers)
  , channel({ 1.0f, 0.0f })
{}

EffectsChannel::~EffectsChannel() {
  for (const auto plugin : vst3Plugins) {
    delete plugin;
  }
  delete this;
}

bool EffectsChannel::addEffect(Vst3::Plugin* plugin) {
  vst3Plugins.push_back(plugin);
  return chainBuffers();
}

bool EffectsChannel::chainBuffers() const {
  const auto firstPlugin = vst3Plugins.front();
  firstPlugin->audioHost->buffers.inputs = inputBuffers;

  for (int i = 1; i < vst3Plugins.size(); ++i) {
    const auto currentPlugin = vst3Plugins.at(i);
    const auto previousPlugin = vst3Plugins.at(i-1);
    const auto toFree = currentPlugin->audioHost->buffers.inputs;
    currentPlugin->audioHost->buffers.inputs = previousPlugin->audioHost->buffers.outputs;
    for (int j = 0; j < 2; j++) {
    	delete toFree[j];
    }
    free(toFree);
  }

  return true;
}

bool EffectsChannel::unchainBuffers() const {
  for (int i = 1; i < vst3Plugins.size(); ++i) {
  	vst3Plugins.at(i)->audioHost->allocateInputBuffers();
  }

  return true;
}

} // Effects
} // Audio
} // Gj
