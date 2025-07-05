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
  constexpr size_t PlaybackSettingsToAudioThread_Count = 3;
  constexpr size_t PlaybackSettingsToAudioThread_RB_SIZE = sizeof(sf_count_t) * PlaybackSettingsToAudioThread_Count;
  constexpr size_t PlaybackSettingsFromAudioThread_Count = 2;
  constexpr size_t PlaybackSettingsFromAudioThread_RB_SIZE = sizeof(sf_count_t) * PlaybackSettingsFromAudioThread_Count;

} // Audio
} // Gj

#endif //CONSTANTS_H
