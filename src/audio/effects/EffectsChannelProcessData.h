//
// Created by ns on 4/19/25.
//

#ifndef EFFECTSCHANNELPROCESSDATA_H
#define EFFECTSCHANNELPROCESSDATA_H

#include <array>
#include <sndfile.hh>
#include "public.sdk/samples/vst-hosting/audiohost/source/media/imediaserver.h"

#include "../Channel.h"
#include "../Constants.h"

namespace Gj {
namespace Audio {
namespace Effects {

struct EffectsChannelProcessData {
  // NOTE: handles at most 100 effects per channel
  int effectCount;
  std::array<void* (*)(Steinberg::Vst::IAudioClient::Buffers, sf_count_t frames), MAX_PLUGINS_PER_CHANNEL> processFuncs;
  std::array<Steinberg::Vst::IAudioClient::Buffers, MAX_PLUGINS_PER_CHANNEL> buffers;
  Channel channelSettings;
};

} // Effects
} // Audio
} // Gj

#endif //EFFECTSCHANNELPROCESSDATA_H
