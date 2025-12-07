//
// Created by ns on 12/5/25.
//

#ifndef BUFFERINDECES_H
#define BUFFERINDECES_H

#include <stddef.h>

#include "../types/Types.h"

namespace Gj {
namespace Audio {
namespace BfrIdx {
// Buffer Indeces

// playbackSettingsFromAudioThread
namespace PSFAT {
static constexpr size_t DEBUG_VALUE = 0;

static constexpr size_t CURRENT_FRAME_ID = 1;
} // PSFAT

// playbackSettingsToAudioThread
namespace PSTAT {
static constexpr size_t USER_SETTING_FRAME_ID_FLAG = 0; // bool
static constexpr size_t NEW_FRAME_ID = 1;

static constexpr size_t PLAYBACK_SPEED = 2;
}


namespace AudCh {
static constexpr int LEFT = 0;

static constexpr int RIGHT = 1;
};

namespace ECS {
static constexpr ChannelIndex factorLL(const ChannelIndex channelIndex) {
  return 4 * channelIndex;
}

static constexpr ChannelIndex factorLR(const ChannelIndex channelIndex) {
  return 4 * channelIndex + 1;
}

static constexpr ChannelIndex factorRL(const ChannelIndex channelIndex) {
  return 4 * channelIndex + 2;
}

static constexpr ChannelIndex factorRR(const ChannelIndex channelIndex) {
  return 4 * channelIndex + 3;
}
}; // ECS

namespace VU {
static constexpr ChannelIndex left(const ChannelIndex channelIndex) {
  return 2 * channelIndex;
}

static constexpr ChannelIndex right(const ChannelIndex channelIndex) {
  return 2 * channelIndex + 1;
}
} // VU

namespace FFT {
static constexpr ChannelIndex left(const ChannelIndex channelIndex) {
  return 2 * channelIndex;
}

static constexpr ChannelIndex right(const ChannelIndex channelIndex) {
  return 2 * channelIndex + 1;
}
} // FFT
} // BfrIdx
} // Audio
} // Gj

#endif //BUFFERINDECES_H
