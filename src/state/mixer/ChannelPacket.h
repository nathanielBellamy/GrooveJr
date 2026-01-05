//
// Created by ns on 12/30/25.
//

#ifndef GJSTATEMIXERCHANNELPACKET_H
#define GJSTATEMIXERCHANNELPACKET_H

#include <string>

#include "../../audio/Constants.h"

#include "PluginSlotPacket.h"

namespace Gj {
namespace State {
namespace Mixer {
struct ChannelPacket {
  ChannelIndex channelIndex = 0;
  PluginSlotPacket plugins[Audio::MAX_PLUGINS_PER_CHANNEL]{};

  ChannelPacket() {
    std::cout << "ChanPacket::Packet()" << std::endl;
    std::fill_n(plugins, Audio::MAX_PLUGINS_PER_CHANNEL, PluginSlotPacket());
  }

  std::string std_str() const {
    std::string res = " Channel " + std::to_string(channelIndex) + " {\n";
    for (const auto& plugin: plugins) {
      res += plugin.std_str() + "\n";
    }
    res += " } \\ END State::Mixer::Channel \n";

    return res;
  }
};

template<class Inspector>
bool inspect(Inspector& f, ChannelPacket& x) {
  return f.object(x).fields(
    f.field("channelIndex", x.channelIndex),
    f.field("plugins", x.plugins)
  );
}
} // Mixer
} // State
} // Gj

#endif //GJSTATEMIXERCHANNELPACKET_H
