//
// Created by ns on 3/16/25.
//

#ifndef EFFECTSCONTAINER_H
#define EFFECTSCONTAINER_H

#include<memory>
#include<vector>

#include <QAction>
#include <QLabel>
#include <QWidget>
#include <QGridLayout>
#include <QHideEvent>
#include <QShowEvent>

#include "../../../../Color.h"
#include "VstWindow.h"
#include "VstWindowSelectButton.h"
#include "../AddEffectButton.h"
#include "../../../../../audio/Mixer.h"
#include "../../../../../Logging.h"
#include "../../../../../types/AtomicStr.h"
#include "../../../../../types/Types.h"

namespace Gj {
namespace Gui {

class EffectsContainer final : public QWidget {

  public:
    EffectsContainer(
      QWidget* parent,
      Audio::Mixer* mixer,
      ChannelIndex channelIndex,
      QAction* addEffectAction,
      QAction* removeEffectAction
    );
    ~EffectsContainer() override;
    void addEffect(PluginIndex newEffectIndex, const AtomicStr& pluginName);

  private:
    Audio::Mixer* mixer;
    ChannelIndex channelIndex;
    QAction* addPluginAction;
    AddEffectButton addPluginButton;
    QAction* removePluginAction;
    QGridLayout grid;
    std::vector<std::shared_ptr<VstWindow>> vstWindows {};
    QAction selectVstWindowAction;
    std::vector<VstWindowSelectButton*> vstWindowSelectButtons {};
    std::vector<QLabel*> vstWindowSelectLabels {};

    void connectActions();
    void clearButtonsAndLabels();
    void setupGrid();
    void setStyle();
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
};

} // Gui
} // Gj


#endif //EFFECTSCONTAINER_H
