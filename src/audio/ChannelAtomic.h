//
// Created by ns on 5/5/25.
//

#ifndef CHANNELATOMIC_H
#define CHANNELATOMIC_H

#include <atomic>

#include "../db/entity/mixer/ChannelEntity.h"

namespace Gj {
namespace Audio {

struct ChannelAtomic {
  std::atomic<float> gain = { 1.0f };
  std::atomic<float> mute = { 0.0f };
  std::atomic<float> solo = { 0.0f };
  std::atomic<float> pan = { 0.0f };

  std::atomic<float> gainL = { 1.0f };
  std::atomic<float> gainR = { 1.0f };
  std::atomic<float> muteL = { 0.0f };
  std::atomic<float> muteR = { 0.0f };
  std::atomic<float> soloL = { 0.0f };
  std::atomic<float> soloR = { 0.0f };
  std::atomic<float> panL = { -1.0f };
  std::atomic<float> panR = { 1.0f };
};

} // Db
} // Gj



#endif //CHANNELATOMIC_H
