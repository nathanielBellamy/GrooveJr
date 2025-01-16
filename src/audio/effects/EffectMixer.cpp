//
// Created by ns on 1/12/25.
//

#include "EffectMixer.h"

namespace Gj {
namespace Audio {
namespace Effects {

EffectMixer::EffectMixer()
    : dryChannel(new Channel())
    , wetChannels(std::vector<EffectChannel*>())
    {}

EffectMixer::~EffectMixer() {
  delete dryChannel;
  for (auto channel : wetChannels) {
    delete channel;
  }
}

bool EffectMixer::addChannel() {
    wetChannels.push_back(new EffectChannel());
}

} // Effects
} // Audio
} // Gj