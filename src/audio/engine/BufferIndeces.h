//
// Created by ns on 12/5/25.
//

#ifndef BUFFERINDECES_H
#define BUFFERINDECES_H

#include <stddef.h>

#include "../../types/Types.h"

namespace Gj {
namespace Audio {
namespace BfrIdx {
// Buffer Indeces

// playbackSettingsFromAudioThread
namespace PSFAT {
constexpr size_t SIZE = 3;
constexpr size_t RB_SIZE = sizeof(sf_count_t) * SIZE;
static constexpr size_t DEBUG_VALUE = 0;

static constexpr size_t CURRENT_FRAME_ID = 1;
static constexpr size_t PROCESS_DATA_CHANGE_FLAG = 2;
} // PSFAT

// playbackSettingsToAudioThread
namespace PSTAT {
static constexpr size_t SIZE = 4;
static constexpr size_t RB_SIZE = sizeof(sf_count_t) * SIZE;
static constexpr size_t USER_SETTING_FRAME_ID_FLAG = 0; // bool
static constexpr size_t NEW_FRAME_ID = 1;

static constexpr size_t PLAYBACK_SPEED = 2;
static constexpr size_t PROCESS_DATA_CHANGE_FLAG = 3;
}


namespace AudCh {
static constexpr int LEFT = 0;

static constexpr int RIGHT = 1;
};


namespace MixerChannel {
namespace ProcessData {
static constexpr size_t RB_SIZE = MAX_MIXER_CHANNELS * sizeof(Mixer::ChannelProcessData);
} // ProcessData

namespace Settings {
static constexpr size_t COUNT = 4;
static constexpr size_t RB_SIZE = COUNT * MAX_MIXER_CHANNELS * sizeof(float);

static constexpr ChannelIndex factorLL(const ChannelIndex channelIndex) {
  return COUNT * channelIndex;
}

static constexpr ChannelIndex factorLR(const ChannelIndex channelIndex) {
  return COUNT * channelIndex + 1;
}

static constexpr ChannelIndex factorRL(const ChannelIndex channelIndex) {
  return COUNT * channelIndex + 2;
}

static constexpr ChannelIndex factorRR(const ChannelIndex channelIndex) {
  return COUNT * channelIndex + 3;
}
} // Settings
}; // MixerChannel

namespace VU {
static constexpr ChannelIndex left(const ChannelIndex channelIndex) {
  return AUDIO_CHANNEL_COUNT * channelIndex;
}

static constexpr ChannelIndex right(const ChannelIndex channelIndex) {
  return AUDIO_CHANNEL_COUNT * channelIndex + 1;
}
} // VU

namespace FFT {
static constexpr ChannelIndex left(const ChannelIndex channelIndex) {
  return AUDIO_CHANNEL_COUNT * channelIndex;
}

static constexpr ChannelIndex right(const ChannelIndex channelIndex) {
  return AUDIO_CHANNEL_COUNT * channelIndex + 1;
}
} // FFT
} // BfrIdx
} // Audio
} // Gj

#endif //BUFFERINDECES_H
