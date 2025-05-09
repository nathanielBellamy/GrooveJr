//
// Created by ns on 5/5/25.
//

#ifndef CHANNELATOMIC_H
#define CHANNELATOMIC_H

#include <atomic>

struct ChannelAtomic {
  std::atomic<float> gain = { 1.0f };
  std::atomic<float> mute = { 0.0f };
  std::atomic<float> solo = { 0.0f };
  // TODO
  // - replace pan with panL, panR
  // - add gainL, gainR
  std::atomic<float> pan = { 0.0f };
  std::atomic<float> panL = { 0.0f };
  std::atomic<float> panR = { 0.0f };
};



#endif //CHANNELATOMIC_H
