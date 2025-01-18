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

bool Mixer::mixDown(int audioDataIndex, float* audioDataBuffer, int audioDataSfChannels, int framesPerBuffer) {
  // called from audio thread
  // do not allocate/free memory!

  // TODO: handle pan/gain
  // TODO: identify all first plugin input buffers?

  std::vector<Effects::Vst3::Plugin*> lastPlugins;

  for (auto effectsChannel : effectsChannels) {
    if (effectsChannel->vst3Plugins.size() == 0)
      break;
    auto firstPlugin = effectsChannel->vst3Plugins.front();
    // populate firstPlugin's input buffers from dry audio signal
    for (int c = 0; c < audioDataSfChannels; c++) {
      for (int i = 0; i < framesPerBuffer; i++) {
        firstPlugin->audioHost->buffers.inputs[c][i] = audioDataBuffer[audioDataIndex + 2 * i + c];
      }
    }

    // process firstPlugin
    firstPlugin->audioHost->vst3Processor->process(firstPlugin->audioHost->buffers, (int64_t) framesPerBuffer);

    // process audio through remaining plugins
    for (int j = 1; j < effectsChannel->vst3Plugins.size(); j++) {
      // note: buffers have been chained
      const auto currPlugin = effectsChannel->vst3Plugins.at(j);
      currPlugin->audioHost->vst3Processor->process(currPlugin->audioHost->buffers, (int64_t) framesPerBuffer);
    }

    lastPlugins.push_back( effectsChannel->vst3Plugins.back() );
  }

  // write lastPlugins outputs to outputBuffer
  for (int i = 0; i < framesPerBuffer; i++) {
      for (int c = 0; c < audioDataSfChannels; c++) {
          float val;
          for (auto plugin : lastPlugins) {
            val += plugin->audioHost->buffers.outputs[c][i];
          }
          outputBuffer[2 * i + c] = val / lastPlugins.size();
      }
  }

  return true;
}

} // Audio
} // Gj