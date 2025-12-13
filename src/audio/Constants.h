//
// Created by ns on 4/19/25.
//

#ifndef GJAUDIOCONSTANTS_H
#define GJAUDIOCONSTANTS_H

#include <stddef.h>
#include <sndfile.h>

namespace Gj {
namespace Audio {
constexpr unsigned int AUDIO_CORE_DECK_COUNT = 3;
constexpr size_t RING_BUFFER_PADDING = 64; //128;

constexpr unsigned int MAX_EFFECTS_CHANNELS = 50;
constexpr unsigned int VU_RING_BUFFER_SIZE = 2 * MAX_EFFECTS_CHANNELS;
constexpr unsigned int MAX_PLUGINS_PER_CHANNEL = 50;
constexpr unsigned int AUDIO_FRAMES_PER_BUFFER_MAX = 2048;
constexpr unsigned int AUDIO_FRAMES_PER_BUFFER_DEFAULT = 256;
constexpr unsigned int FFT_EQ_TIME_SIZE = 1024;
constexpr unsigned int FFT_EQ_FREQ_SIZE = 513;
constexpr size_t FFT_EQ_RING_BUFFER_SIZE = 2 * FFT_EQ_FREQ_SIZE * sizeof(float);
constexpr unsigned int FFT_PV_TIME_SIZE = 1024;
constexpr unsigned int FFT_PV_TIME_SIZEF = 1024.0f;
constexpr unsigned int FFT_PV_HOP_COUNT = 4;
constexpr unsigned int FFT_PV_FREQ_SIZE = 513;
constexpr unsigned int FFT_PV_HOP_ANALYSIS = 256;
constexpr float FFT_PV_HOP_ANALYSISF = 256.0f;
constexpr unsigned int FFT_PV_OLA_BUFFER_SIZE = FFT_PV_TIME_SIZE + (FFT_PV_HOP_COUNT - 1) * FFT_PV_HOP_ANALYSIS;
constexpr float PI = 3.14159265358979323846;
constexpr float TWO_PI = 2.0f * PI;

constexpr size_t EffectsSettings_RB_SIZE = 2 * MAX_EFFECTS_CHANNELS * sizeof(float);
constexpr size_t PlaybackSettingsToAudioThread_Count = 3;
constexpr size_t PlaybackSettingsToAudioThread_RB_SIZE = sizeof(sf_count_t) * PlaybackSettingsToAudioThread_Count;
constexpr size_t PlaybackSettingsFromAudioThread_Count = 2;
constexpr size_t PlaybackSettingsFromAudioThread_RB_SIZE = sizeof(sf_count_t) * PlaybackSettingsFromAudioThread_Count;
} // Audio
} // Gj

#endif //GJAUDIOCONSTANTS_H
