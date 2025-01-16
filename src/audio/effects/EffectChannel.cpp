//
// Created by ns on 1/12/25.
//

#include "EffectChannel.h"


namespace Gj {
namespace Audio {
namespace Effects {

EffectChannel::~EffectChannel() {
  for (auto plugin : vst3Plugins) {
    delete plugin;
  }
  delete this;
}

bool EffectChannel::chainBuffers() {
  for (int i = 1; i < vst3Plugins.size(); ++i) {
    auto currentPlugin = vst3Plugins.at(i);
    auto previousPlugin = vst3Plugins.at(i-1);
    auto toFree = currentPlugin->audioHost->buffers.inputs;
    currentPlugin->audioHost->buffers.inputs = previousPlugin->audioHost->buffers.outputs;
    for (int i = 0; i < 2; i++) {
    	free(toFree[i]);
    }
    free(toFree);
  }

  return true;
}

bool EffectChannel::unchainBuffers() {
  for (int i = 1; i < vst3Plugins.size(); ++i) {
  	vst3Plugins.at(i)->audioHost->allocateInputBuffers();
  }

  return true;
}

} // Effects
} // Audio
} // Gj
