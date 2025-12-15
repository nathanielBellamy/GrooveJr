//
// Created by ns on 6/7/25.
//

#include "ChannelEntity.h"

namespace Gj {
namespace Db {
ChannelEntity::ChannelEntity(
  const ID id,
  const ChannelIndex channelIndex,
  const AtomicStr& name,
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
  , name(name)
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
  , panR(panR) {
}

ChannelEntity ChannelEntity::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const ChannelIndex channelIndex = sqlite3_column_int(stmt, 1);
  const unsigned char* name = sqlite3_column_text(stmt, 2);
  const auto gain = static_cast<float>(sqlite3_column_double(stmt, 3));
  const auto mute = static_cast<float>(sqlite3_column_double(stmt, 4));
  const auto solo = static_cast<float>(sqlite3_column_double(stmt, 5));
  const auto pan = static_cast<float>(sqlite3_column_double(stmt, 6));
  const auto gainL = static_cast<float>(sqlite3_column_double(stmt, 7));
  const auto gainR = static_cast<float>(sqlite3_column_double(stmt, 8));
  const auto muteL = static_cast<float>(sqlite3_column_double(stmt, 9));
  const auto muteR = static_cast<float>(sqlite3_column_double(stmt, 10));
  const auto soloL = static_cast<float>(sqlite3_column_double(stmt, 11));
  const auto soloR = static_cast<float>(sqlite3_column_double(stmt, 12));
  const auto panL = static_cast<float>(sqlite3_column_double(stmt, 13));
  const auto panR = static_cast<float>(sqlite3_column_double(stmt, 14));

  return {
    id,
    channelIndex,
    AtomicStr(reinterpret_cast<const char*>(name)),
    gain,
    mute,
    solo,
    pan,
    gainL,
    gainR,
    muteL,
    muteR,
    soloL,
    soloR,
    panL,
    panR
  };
}

std::vector<ChannelEntity> ChannelEntity::baseChannels() {
  const ChannelEntity baseMain(
    0,
    0,
    AtomicStr("Main"),
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    -1.0f,
    1.0f
  );

  const ChannelEntity baseChannel1(
    0,
    1,
    AtomicStr("==1=="),
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    -1.0f,
    1.0f
  );

  return {
    baseMain,
    baseChannel1
  };
}
} // Db
} // Gj
