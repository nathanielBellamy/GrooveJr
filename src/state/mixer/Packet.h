//
// Created by ns on 12/30/25.
//

#ifndef GJSTATEMIXERPACKET_H
#define GJSTATEMIXERPACKET_H

#include <string>

#include "../../audio/Constants.h"

#include "ChannelPacket.h"

namespace Gj {
namespace State {
namespace Mixer {
struct Packet {
  ChannelPacket channels[Audio::MAX_MIXER_CHANNELS]{};

  Packet() {
    std::cout << "MixPacket::Packet()" << std::endl;
    std::fill_n(channels, Audio::MAX_MIXER_CHANNELS, ChannelPacket());
  }

  std::string std_str() const {
    std::string res = "\n State::Mixer::Packet { \n";
    for (const auto& channel: channels) {
      res += "/n" + channel.std_str() + "\n";
    }
    res += " } \\ END State::Mixer::Packet \n";

    return res;
  }
};

template<class Inspector>
bool inspect(Inspector& f, Packet& x) {
  return f.object(x).fields(
    f.field("channels", x.channels)
  );
}
} // Mixer
} // State
} // Gj
#endif //MIXERPACKET_H
