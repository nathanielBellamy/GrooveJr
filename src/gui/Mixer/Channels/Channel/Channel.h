//
// Created by ns on 3/7/25.
//

#ifndef GJGUIMIXERCHANNEL_H
#define GJGUIMIXERCHANNEL_H

#include <atomic>
#include <optional>

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"
#include "../../../../messaging/atoms.h"

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSlider>
#include <QWidget>
#include <QUrl>

#include "../../../../state/Core.h"

#include "../../../../audio/mixer/Core.h"
#include "../../../../audio/Math.h"
#include "Plugins/PluginSlots.h"
#include "MuteSoloContainer.h"
#include "RemoveChannelButton.h"
#include "VuMeter.h"
#include "./Plugins/PluginsContainer.h"
#include "../../../Shared/VstSelect.h"
#include "../../../Color.h"

#include "../../../../Logging.h"

using namespace caf;

namespace Gj {
namespace Gui {
namespace Mixer {
class Channel final : public QWidget {
public:
  Channel(
    QWidget* parent,
    actor_system& actorSystem,
    Audio::Mixer::Core* mixer,
    ChannelIndex channelIndex,
    QAction* removeChannelAction,
    QAction* muteChannelAction,
    QAction* muteLChannelAction,
    QAction* muteRChannelAction,
    QAction* soloChannelAction,
    QAction* soloLChannelAction,
    QAction* soloRChannelAction,
    std::atomic<float>* vuPtr
  );

  ~Channel() override;

  void hydrateState(const State::Packet& statePacket, ChannelIndex newChannelIndex);

  void updateShowRemoveChannelButton(bool val);

  void setMute(float val);

  void setMuteL(float val);

  void setMuteR(float val);

  void setSolo(float val);

  void setSoloL(float val);

  void setSoloR(float val);

  void setPlugins();

  void addPlugin(std::optional<PluginIndex> pluginIndex);

  ChannelIndex channelIndex;

private:
  actor_system& actorSystem;
  strong_actor_ptr appStateManagerPtr;
  Audio::Mixer::Core* mixer;
  std::atomic<float>* vuPtr;
  VuMeter vuMeter;
  QAction* removeChannelAction;
  RemoveChannelButton removeChannelButton;
  QAction addPluginAction;
  AddPluginButton addPluginButton;
  QAction openPluginsContainer;
  VstSelect vstSelect;
  QUrl vstUrl;
  QAction replacePluginAction;
  QAction removePluginAction;
  PluginsContainer pluginsContainer;
  QGridLayout grid;
  QLabel title;
  QSlider gainSlider;
  QLabel gainLabel;
  QSlider gainLSlider;
  QLabel gainLLabel;
  QSlider gainRSlider;
  QLabel gainRLabel;
  QSlider panSlider;
  QLabel panLabel;
  QSlider panLSlider;
  QLabel panLLabel;
  QSlider panRSlider;
  QLabel panRLabel;
  QScrollArea pluginSlotsScrollArea;
  PluginSlots pluginSlots;
  MuteSoloContainer muteSoloContainer;

  void setStyle();

  void setupGrid();

  void setupTitle();

  void setupGainSlider(float gain);

  void setupGainLSlider(float gainL);

  void setupGainRSlider(float gainR);

  void setupPanSlider(float pan);

  void setupPanLSlider(float panL);

  void setupPanRSlider(float panR);

  void connectActions();

  void setupPluginSlotsScrollArea();
};
} // Mixer
} // Gui
} // Gj

#endif //GJGUIMIXERCHANNEL_H
