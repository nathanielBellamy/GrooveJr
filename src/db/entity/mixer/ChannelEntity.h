//
// Created by ns on 6/7/25.
//

#ifndef CHANNELENTITY_H
#define CHANNELENTITY_H

#include <vector>

#include <sqlite3.h>
#include "../../../types/AtomicStr.h"
#include "../../Types.h"

namespace Gj {
namespace Db {

struct ChannelEntity {
  ID id;
  ChannelIndex channelIndex;
  AtomicStr name;
  float gain;
  float mute;
  float solo;
  float pan;

  float gainL;
  float gainR;
  float muteL;
  float muteR;
  float soloL;
  float soloR;
  float panL;
  float panR;

  ChannelEntity(
    ID id,
    ChannelIndex channelIndex,
    const AtomicStr& name,
    float gain,
    float mute,
    float solo,
    float pan,

    float gainL,
    float gainR,
    float muteL,
    float muteR,
    float soloL,
    float soloR,
    float panL,
    float panR
  );

  bool operator=(const ChannelEntity& rhs) const {
    return id == rhs.id;
  }
  bool operator<(const ChannelEntity& rhs) const {
    return channelIndex < rhs.channelIndex;
  }

  static ChannelEntity deser(sqlite3_stmt* stmt);
  static std::vector<ChannelEntity> baseChannels();
};

} // Db
} // Gj


#endif //CHANNELENTITY_H
