//
// Created by ns on 6/7/25.
//

#ifndef CHANNELENTITY_H
#define CHANNELENTITY_H

namespace Gj {
namespace Db {

struct ChannelEntity {
  int id;
  int channelIndex;
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

};

} // Db
} // Gj


#endif //CHANNELENTITY_H
