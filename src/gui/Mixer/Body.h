//
// Created by ns on 2/28/25.
//

#ifndef MIXERWINDOW_H
#define MIXERWINDOW_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include <jack/ringbuffer.h>

#include "./Channels/MainChannelContainer.h"
#include "Channels/ChannelsContainer.h"

#include "../../state/Core.h"
#include "../../audio/mixer/Core.h"
#include "../Color.h"

using namespace caf;

namespace Gj {
namespace Gui {
namespace Mixer {
class Body final : public QWidget {
public:
  explicit Body(QWidget* parent, actor_system& actorSystem, Audio::Mixer::Core* mixer);

  ~Body();

  void hydrateState(const State::Packet& statePacket);

  void setChannels();

  // void setPlugins();

  void setVuRingBuffer(jack_ringbuffer_t* ringBuffer) { vuRingBuffer = ringBuffer; }

private:
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;
  QAction* muteChannelAction;
  QAction* muteLChannelAction;
  QAction* muteRChannelAction;
  QAction* soloChannelAction;
  QAction* soloLChannelAction;
  QAction* soloRChannelAction;
  QGridLayout* grid;
  QLabel* title;
  MainChannelContainer* mainChannelContainer;
  ChannelsContainer* channelsContainer;

  std::thread vuWorker;
  std::atomic<bool> vuWorkerRunning = false;
  std::atomic<bool> stopVuWorker;
  jack_ringbuffer_t* vuRingBuffer;
  float vuBufferIn[Audio::VU_RING_BUFFER_SIZE]{};
  int vuAvgIndex = 0;
  float vuBufferAvg[VU_METER_AVG_SIZE][Audio::VU_RING_BUFFER_SIZE]{};
  std::atomic<float> vuBuffer[Audio::VU_RING_BUFFER_SIZE]{};

  Result vuWorkerStart();

  Result vuWorkerStop();

  void setStyle();

  void setupGrid();

  void connectActions();
};
} // Mixer
} // Gui
} // Gj

#endif //MIXERWINDOW_H
