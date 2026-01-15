//
// Created by ns on 3/16/25.
//

#ifndef GJGUIMIXERPLUGINSCONTAINER_H
#define GJGUIMIXERPLUGINSCONTAINER_H

#include<memory>

#include <QAction>
#include <QLabel>
#include <QWidget>
#include <QGridLayout>
#include <QHideEvent>
#include <QShowEvent>

#include "../../../../Color.h"
#include "VstWindow.h"
#include "VstWindowSelectButton.h"
#include "AddPluginButton.h"
#include "../../../../../audio/mixer/Core.h"
#include "../../../../../audio/Constants.h"
#include "../../../../../Logging.h"
#include "../../../../../types/AtomicStr.h"
#include "../../../../../types/Types.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class PluginsContainer final : public QWidget {
public:
  PluginsContainer(
    QWidget* parent,
    Audio::Mixer::Core* mixer,
    ChannelIndex channelIndex,
    QAction* addPluginAction,
    QAction* removePluginAction
  );

  ~PluginsContainer() override;

  Result hydrateState(const State::Packet& statePacket, ChannelIndex newChannelIndex);

  void addPlugin(PluginIndex newPluginIndex, const AtomicStr& pluginName);

private:
  Audio::Mixer::Core* mixer;
  ChannelIndex channelIndex;
  QAction* addPluginAction;
  AddPluginButton addPluginButton;
  QAction* removePluginAction;
  QGridLayout grid;
  std::shared_ptr<VstWindow> vstWindows[Audio::MAX_PLUGINS_PER_CHANNEL]{};
  QAction selectVstWindowAction;
  std::unique_ptr<VstWindowSelectButton> vstWindowSelectButtons[Audio::MAX_PLUGINS_PER_CHANNEL]{};
  std::unique_ptr<QLabel> vstWindowSelectLabels[Audio::MAX_PLUGINS_PER_CHANNEL]{};

  void connectActions();

  void clearButtonsAndLabels();

  void setupGrid();

  void setStyle();

  void showEvent(QShowEvent* event) override;

  void hideEvent(QHideEvent* event) override;

  bool hasPluginAt(PluginIndex pluginIndex) const {
    return pluginIndex < Audio::MAX_PLUGINS_PER_CHANNEL
           && vstWindows[pluginIndex] != nullptr;
  };
};
} // Mixer
} // Gui
} // Gj


#endif //GJGUIMIXERPLUGINSCONTAINER_H
