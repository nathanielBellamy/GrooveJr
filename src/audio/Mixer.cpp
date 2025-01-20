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
    , channelCount(1)
    {
      // TODO: don't assume stereo
      outputBuffer = new float[AUDIO_BUFFER_FRAMES * 2];
    }

Mixer::~Mixer() {
  for (const auto channel : effectsChannels) {
    delete channel;
  }
  delete outputBuffer;
  delete this;
}

bool Mixer::addEffectsChannel() {
    effectsChannels.push_back(new Effects::EffectsChannel());
    channelCount++;
    return true;
}

bool Mixer::removeEffectsChannel(const int idx) {
  effectsChannels.erase(effectsChannels.begin() + idx);
  channelCount--;
  return true;
}

bool Mixer::addEffectToChannel(const int idx, Effects::Vst3::Plugin* effect) const {
  return effectsChannels.at(idx)->addEffect(effect);
}

bool Mixer::mixDown(const int audioDataIndex, const float* audioDataBuffer, const int audioDataSfChannels, const int framesPerBuffer) const {
  // called from audio thread
  // do not allocate/free memory!

  // TODO: handle pan/gain
  // TODO: identify all first plugin input buffers

  // write dry channel output buffer
  for (int c = 0; c < audioDataSfChannels; c++) {
    for (int i = 0; i < framesPerBuffer; i++) {
      outputBuffer[2 * i + c] = audioDataBuffer[audioDataIndex + 2 * i + c] / channelCount;
    }
  }

  for (const auto effectsChannel : effectsChannels) {
    if (channelCount == 1) // dry channel only
      break;
    auto firstPlugin = effectsChannel->vst3Plugins.front();
    // populate firstPlugin's input buffers from dry audio signal
    for (int c = 0; c < audioDataSfChannels; c++) {
      for (int i = 0; i < framesPerBuffer; i++) {
        firstPlugin->audioHost->buffers.inputs[c][i] = audioDataBuffer[audioDataIndex + 2 * i + c];
      }
    }

    // process audio plugins
    for (int j = 0; j < effectsChannel->vst3Plugins.size(); j++) {
      // note: buffers have been chained
      const auto currPlugin = effectsChannel->vst3Plugins.at(j);
      currPlugin->audioHost->vst3Processor->process(currPlugin->audioHost->buffers, (int64_t) framesPerBuffer);
    }

    for (int c = 0; c < audioDataSfChannels; c++) {
      for (int i = 0; i < framesPerBuffer; i++) {
        outputBuffer[2 * i + c] += effectsChannel->vst3Plugins.back()->audioHost->buffers.outputs[c][i] / channelCount;
      }
    }
  }

  return true;
}

} // Audio
} // Gj