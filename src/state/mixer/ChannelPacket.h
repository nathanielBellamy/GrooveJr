//
// Created by ns on 12/30/25.
//

#ifndef GJSTATEMIXERCHANNELPACKET_H
#define GJSTATEMIXERCHANNELPACKET_H

#include "../../audio/Constants.h"

#include "PluginPacket.h"

namespace Gj {
namespace State {
namespace Mixer {
struct ChannelPacket {
  PluginPacket plugins[Audio::MAX_PLUGINS_PER_CHANNEL];
};
} // Mixer
} // State
} // Gj

#endif //GJSTATEMIXERCHANNELPACKET_H
