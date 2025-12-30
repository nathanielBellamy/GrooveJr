//
// Created by ns on 12/30/25.
//

#ifndef GJSTATEMIXERPACKET_H
#define GJSTATEMIXERPACKET_H

#include "../../audio/Constants.h"

#include "ChannelPacket.h"

namespace Gj {
namespace State {
namespace Mixer {
struct Packet {

  ChannelPacket channels[Audio::MAX_MIXER_CHANNELS];

};

} // Mixer
} // State
} // Gj
#endif //MIXERPACKET_H
