//
// Created by ns on 6/7/25.
//

#ifndef CHANNELENTITY_H
#define CHANNELENTITY_H

#include <string>

#include <sqlite3.h>

namespace Gj {
namespace Db {

struct ChannelEntity {
  int id;
  int channelIndex;
  std::string name;
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
    int id,
    int channelIndex,
    std::string name,
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
};

} // Db
} // Gj


#endif //CHANNELENTITY_H
