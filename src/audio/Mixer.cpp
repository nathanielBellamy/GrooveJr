//
// Created by ns on 1/12/25.
//

#include "Mixer.h"

namespace Gj {
namespace Audio {

Mixer::Mixer()
    : mainChannel({ 1.0f, 0.0f })
    , dryChannel({ 1.0f, 0.0f })
    , effectsChannels(std::vector<Effects::EffectsChannel*>())
    {}

Mixer::~Mixer() {
  for (auto channel : effectsChannels) {
    delete channel;
  }
}

bool Mixer::addEffectsChannel() {
    effectsChannels.push_back(new Effects::EffectsChannel());
    return true;
}

bool Mixer::removeEffectsChannel(int index) {
  effectsChannels.erase(effectsChannels.begin() + index);
  return true;
}

} // Audio
} // Gj