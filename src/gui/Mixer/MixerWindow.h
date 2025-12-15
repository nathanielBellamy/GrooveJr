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

#include "./Channel/MainChannelContainer.h"
#include "./Channel/EffectsChannel/EffectsChannelsContainer.h"

#include "../../AppState.h"
#include "../../audio/mixer/Core.h"
#include "../Color.h"

using namespace caf;

namespace Gj {
namespace Gui {
class MixerWindow final : public QWidget {
public:
  explicit MixerWindow(QWidget* parent, actor_system& actorSystem, Audio::Mixer::Core* mixer);

  ~MixerWindow();

  void hydrateState(const AppStatePacket& appStatePacket);

  void setChannels();

  void setEffects();

  void setVuRingBuffer(jack_ringbuffer_t* ringBuffer) { vuRingBuffer = ringBuffer; }

private:
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;
  QAction muteChannelAction;
  QAction muteLChannelAction;
  QAction muteRChannelAction;
  QAction soloChannelAction;
  QAction soloLChannelAction;
  QAction soloRChannelAction;
  QGridLayout grid;
  QLabel title;
  MainChannelContainer mainChannelContainer;
  EffectsChannelsContainer effectsChannelsContainer;

  std::thread vuWorker;
  std::atomic<bool> vuWorkerRunning = false;
  std::atomic<bool> stopVuWorker;
  jack_ringbuffer_t* vuRingBuffer;
  float vuBufferIn[Audio::VU_RING_BUFFER_SIZE]{0.0f};
  int vuAvgIndex = 0;
  float vuBufferAvg[VU_METER_AVG_SIZE][Audio::VU_RING_BUFFER_SIZE]{-100.0f};
  std::atomic<float> vuBuffer[Audio::VU_RING_BUFFER_SIZE]{-100.0f};

  Result vuWorkerStart();

  Result vuWorkerStop();

  void setStyle();

  void setupGrid();

  void connectActions();
};
} // Gui
} // Gj

#endif //MIXERWINDOW_H
