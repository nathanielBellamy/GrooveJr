//
// Created by ns on 3/8/25.
//

#ifndef MAINDRYCONTAINER_H
#define MAINDRYCONTAINER_H

#include <atomic>
#include <memory>

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QWidget>

#include "../../../AppState.h"
#include "EffectsChannel/EffectsChannel.h"
#include "../../../audio/mixer/Core.h"
#include "../../Color.h"

using namespace caf;

namespace Gj {
namespace Gui {
class MainChannelContainer final : public QWidget {
public:
  MainChannelContainer(
    QWidget* parent,
    actor_system& actorSystem,
    Audio::Mixer::Core* mixer,
    QAction* muteChannelAction,
    QAction* muteLChannelAction,
    QAction* muteRChannelAction,
    QAction* soloChannelAction,
    QAction* soloLChannelAction,
    QAction* soloRChannelAction,
    std::atomic<float>* vuPtr
  );

  void hydrateState(const AppStatePacket& appState) const;

  void setMute(const float val) const {
    mainChannel->setMute(val);
  };

  void setMuteL(const float val) const {
    mainChannel->setMuteL(val);
  };

  void setMuteR(const float val) const {
    mainChannel->setMuteR(val);
  };

  void setSolo(const float val) const {
    mainChannel->setSolo(val);
  };

  void setSoloL(const float val) const {
    mainChannel->setSoloL(val);
  };

  void setSoloR(const float val) const {
    mainChannel->setSoloR(val);
  };

  void setEffects() const;

  void setChannel();

private:
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;
  QGridLayout grid;
  std::atomic<float>* vuPtr;
  QAction* muteChannelAction;
  QAction* muteLChannelAction;
  QAction* muteRChannelAction;
  QAction* soloChannelAction;
  QAction* soloLChannelAction;
  QAction* soloRChannelAction;
  std::unique_ptr<EffectsChannel> mainChannel;

  void setStyle();

  void setupGrid();
};
} // Gui
} // Gj


#endif //MAINDRYCONTAINER_H
