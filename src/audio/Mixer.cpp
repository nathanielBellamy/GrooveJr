//
// Created by ns on 1/12/25.
//

#include "Mixer.h"

namespace Gj {
namespace Audio {

Mixer::Mixer()
    : mainChannel({ 1.0f, 0.0f })
    , dryChannel({ 1.0f, 0.0f })
    , channelCount(1)
    , effectsChannels(std::vector<Effects::EffectsChannel*>())
    {
      // TODO: don't assume stereo
      inputBuffers = static_cast<float**>(
          malloc(2 * AUDIO_BUFFER_FRAMES * sizeof(float))
      );

      if (inputBuffers == nullptr)
        std::cout << "Unable to allocate memory for Mixer.inputBuffers" << std::endl;

      for (int c = 0; c < 2; c++) {
        inputBuffers[c] = new float[AUDIO_BUFFER_FRAMES];
      }

      outputBuffer = new float[AUDIO_BUFFER_FRAMES * 2];
    }

Mixer::~Mixer() {
  for (const auto channel : effectsChannels) {
    delete channel;
  }

  for (auto i = 0; i < 2; i++) {
    delete inputBuffers[i];
  }
  free(inputBuffers);

  delete outputBuffer;
  delete this;
}

bool Mixer::addEffectsChannel() {
    effectsChannels.push_back(new Effects::EffectsChannel(inputBuffers));
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

bool Mixer::mixDown(
  const int audioDataIndex,
  const float* audioDataBuffer,
  const int audioDataSfChannels,
  const int framesPerBuffer
  ) const {
  // called from audio thread
  // do not allocate/free memory!

  // TODO: handle pan/gain

  for (int c = 0; c < audioDataSfChannels; c++) { // TODO: at the moment, we can assume audioDataSfChannels = 2
    for (int i = 0; i < framesPerBuffer; i++) {
      const auto val = audioDataBuffer[audioDataIndex + 2 * i + c];
      // write dry channel output buffer
      outputBuffer[2 * i + c] = dryChannel.gain * val / channelCount;

      // de-interlace audio into shared effects input buffers
      inputBuffers[c][i] = val;
    }
  }

  for (const auto effectsChannel : effectsChannels) {
    if (channelCount == 1) // dry channel only
      break;

    // process audio plugins
    for (int j = 0; j < effectsChannel->vst3Plugins.size(); j++) {
      // note: buffers have been chained
      const auto currPlugin = effectsChannel->vst3Plugins.at(j);
      currPlugin->audioHost->vst3Processor->process(currPlugin->audioHost->buffers, (int64_t) framesPerBuffer);
    }

    // write processed audio to outputBuffer
    for (int c = 0; c < audioDataSfChannels; c++) {
      for (int i = 0; i < framesPerBuffer; i++) {
        outputBuffer[2 * i + c] +=
          ( effectsChannel->channel.gain * effectsChannel->vst3Plugins.back()->audioHost->buffers.outputs[c][i] )/ channelCount;
      }
    }
  }

  return true;
}

} // Audio
} // Gj