//
// Created by ns on 6/7/25.
//

#include "ChannelEntity.h"

namespace Gj {
namespace Db {

ChannelEntity::ChannelEntity(
    const int id,
    const int channelIndex,
    const float gain,
    const float mute,
    const float solo,
    const float pan,
    const float gainL,
    const float gainR,
    const float muteL,
    const float muteR,
    const float soloL,
    const float soloR,
    const float panL,
    const float panR)
    : id(id)
    , channelIndex(channelIndex)
    , gain(gain)
    , mute(mute)
    , solo(solo)
    , pan(pan)
    , gainL(gainL)
    , gainR(gainR)
    , muteL(muteL)
    , muteR(muteR)
    , soloL(soloL)
    , soloR(soloR)
    , panL(panL)
    , panR(panR)
    {}

} // Db
} // Gj