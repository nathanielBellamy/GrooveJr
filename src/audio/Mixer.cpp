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
    {
      // TODO: don't assume stereo
      outputBuffer = new float[AUDIO_BUFFER_FRAMES * 2];
    }

Mixer::~Mixer() {
  for (auto channel : effectsChannels) {
    delete channel;
  }
  delete outputBuffer;
  delete this;
}

bool Mixer::addEffectsChannel() {
    effectsChannels.push_back(new Effects::EffectsChannel());
    return true;
}

bool Mixer::removeEffectsChannel(int index) {
  effectsChannels.erase(effectsChannels.begin() + index);
  return true;
}

bool Mixer::addEffectToChannel(int idx, Effects::Vst3::Plugin* effect) {
  return effectsChannels.at(idx)->addEffect(effect);
}

bool Mixer::mixDown(AUDIO_DATA* audioData, int framesPerBuffer) {

  // TODO: fix and organize logic into appropriate classes
      auto firstPlugin = audioData->vst3Plugins.front();
      // populate firstPlugin's input buffers from dry audio signal
      for (c = 0; c < audioData->sfinfo.channels; c++) {
          for (i = 0; i < framesPerBuffer; i++) {
              firstPlugin->audioHost->buffers.inputs[c][i] = audioData->buffer[audioData->index + 2 * i + c];
          }
      }
      // process firstPlugin
      firstPlugin->audioHost->vst3Processor->process(firstPlugin->audioHost->buffers, (int64_t) framesPerBuffer);

      // process audio through remaining plugins
      for (int j = 1; j < audioData->vst3Plugins.size(); j++) {
          // note: buffers have been chained
          const auto currPlugin = audioData->vst3Plugins.at(j);
          currPlugin->audioHost->vst3Processor->process(currPlugin->audioHost->buffers, (int64_t) framesPerBuffer);
      }

      const auto lastPlugin = audioData->vst3Plugins.back();
      // write lastPlugin output buffers to audio out
      for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
          for (c = 0; c < audioData->sfinfo.channels; c++) {
              *out++ = lastPlugin->audioHost->buffers.outputs[c][i] * audioData->volume;
          }
      }
  return true;
}

} // Audio
} // Gj