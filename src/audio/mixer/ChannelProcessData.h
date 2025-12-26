//
// Created by ns on 4/19/25.
//

#ifndef GJGUIMIXERCHANNELPROCESSDATA_H
#define GJGUIMIXERCHANNELPROCESSDATA_H

#include <functional>
#include <sndfile.hh>
#include "public.sdk/samples/vst-hosting/audiohost/source/media/imediaserver.h"

#include "../Constants.h"
#include "../../types/Types.h"

namespace Gj {
namespace Audio {
namespace Mixer {
struct ChannelProcessData {
  std::function<bool(Steinberg::Vst::IAudioClient::Buffers&, int64_t frames)> processFuncs[MAX_PLUGINS_PER_CHANNEL]{};
  Steinberg::Vst::IAudioClient::Buffers buffers[MAX_PLUGINS_PER_CHANNEL]{};
  bool processingEnabledFor[MAX_PLUGINS_PER_CHANNEL]{};
  PluginIndex pluginCount = 0;

  ChannelProcessData() {
    std::fill_n(
      processFuncs,
      MAX_PLUGINS_PER_CHANNEL,
      [](auto&&, auto&&) { return true; }
    );
  }
};
} // Mixer
} // Audio
} // Gj

#endif //GJGUIMIXERCHANNELPROCESSDATA_H
