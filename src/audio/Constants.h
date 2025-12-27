//
// Created by ns on 4/19/25.
//

#ifndef GJAUDIOCONSTANTS_H
#define GJAUDIOCONSTANTS_H

#include <stddef.h>
#include <sndfile.h>

namespace Gj {
namespace Audio {
static constexpr unsigned int AUDIO_CORE_DECK_COUNT = 3;
static constexpr size_t RING_BUFFER_PADDING = 64; //128;

static constexpr unsigned int MAX_MIXER_CHANNELS = 50;
static constexpr unsigned int MAX_MIXER_CHANNELS_SETTINGS_COUNT = MAX_MIXER_CHANNELS * 4;
// * 4 comes from factorLL, factorLR, factorRL, factorRR
static constexpr unsigned int VU_RING_BUFFER_SIZE = 2 * MAX_MIXER_CHANNELS;
static constexpr unsigned int MAX_PLUGINS_PER_CHANNEL = 50;
static constexpr unsigned int AUDIO_CHANNEL_COUNT = 2;
static constexpr unsigned int AUDIO_FRAMES_PER_BUFFER_MAX = 2048;
static constexpr unsigned int AUDIO_FRAMES_PER_BUFFER_DEFAULT = 256;
static constexpr unsigned int FFT_EQ_TIME_SIZE = 1024;
static constexpr unsigned int FFT_EQ_FREQ_SIZE = 513;
static constexpr size_t FFT_EQ_RING_BUFFER_SIZE = 2 * FFT_EQ_FREQ_SIZE * sizeof(float);
static constexpr unsigned int FFT_PV_TIME_SIZE = 1024;
static constexpr unsigned int FFT_PV_TIME_SIZEF = 1024.0f;
static constexpr unsigned int FFT_PV_HOP_COUNT = 4;
static constexpr unsigned int FFT_PV_FREQ_SIZE = 513;
static constexpr unsigned int FFT_PV_HOP_ANALYSIS = 256;
static constexpr float FFT_PV_HOP_ANALYSISF = 256.0f;
static constexpr unsigned int FFT_PV_OLA_BUFFER_SIZE = FFT_PV_TIME_SIZE + (FFT_PV_HOP_COUNT - 1) * FFT_PV_HOP_ANALYSIS;
static constexpr size_t FFT_FREQ_BUFFERS_BUFFER_SIZE = AUDIO_FRAMES_PER_BUFFER_MAX * AUDIO_CHANNEL_COUNT;
static constexpr float PI = 3.14159265358979323846;
static constexpr float TWO_PI = 2.0f * PI;
} // Audio
} // Gj

#endif //GJAUDIOCONSTANTS_H
