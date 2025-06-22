//
// Created by ns on 4/19/25.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Gj {
namespace Audio {

  constexpr unsigned int MAX_EFFECTS_CHANNELS = 50;
  constexpr unsigned int MAX_PLUGINS_PER_CHANNEL = 50;
  constexpr unsigned int MAX_AUDIO_FRAMES_PER_BUFFER = 1024;

  constexpr size_t EffectsSettings_RB_SIZE = 2 * MAX_EFFECTS_CHANNELS * sizeof(float);
  constexpr size_t PlaybackSettings_RB_SIZE = 2 * sizeof(sf_count_t);

} // Audio
} // Gj

#endif //CONSTANTS_H
