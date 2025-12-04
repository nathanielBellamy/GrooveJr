//
// Created by ns on 4/19/25.
//

#ifndef EFFECTSCHANNELPROCESSDATA_H
#define EFFECTSCHANNELPROCESSDATA_H

#include <array>
#include <functional>
#include <sndfile.hh>
#include "public.sdk/samples/vst-hosting/audiohost/source/media/imediaserver.h"

#include "../Constants.h"
#include "../../types/Types.h"

namespace Gj {
namespace Audio {
namespace Effects {

struct EffectsChannelProcessData {
  PluginIndex pluginCount;
  std::array<std::function<bool(Steinberg::Vst::IAudioClient::Buffers&, int64_t frames)>, MAX_PLUGINS_PER_CHANNEL> processFuncs{};
  std::array<Steinberg::Vst::IAudioClient::Buffers, MAX_PLUGINS_PER_CHANNEL> buffers{};

  EffectsChannelProcessData()
    : pluginCount(0)
    {};
};

} // Effects
} // Audio
} // Gj

#endif //EFFECTSCHANNELPROCESSDATA_H
