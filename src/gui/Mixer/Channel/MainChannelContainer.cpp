//
// Created by ns on 3/8/25.
//

#include "MainChannelContainer.h"

namespace Gj {
namespace Gui {

MainChannelContainer::MainChannelContainer(
  QWidget* parent,
  actor_system& actorSystem,
  Audio::Mixer* mixer,
  QAction* muteChannelAction,
  QAction* muteLChannelAction,
  QAction* muteRChannelAction,
  QAction* soloChannelAction,
  QAction* soloLChannelAction,
  QAction* soloRChannelAction,
  std::atomic<float>* vuPtr
  )
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , grid(this)
  , vuPtr(vuPtr)
  , muteChannelAction(muteChannelAction)
  , muteLChannelAction(muteLChannelAction)
  , muteRChannelAction(muteRChannelAction)
  , soloChannelAction(soloChannelAction)
  , soloLChannelAction(soloLChannelAction)
  , soloRChannelAction(soloRChannelAction)
  , mainChannel(std::make_unique<EffectsChannel>(
    this, actorSystem, mixer, 0, nullptr,
    muteChannelAction, muteLChannelAction, muteRChannelAction,
    soloChannelAction, soloLChannelAction, soloRChannelAction,
    vuPtr
  ))
  {
  setupGrid();
  setStyle();

  Logging::write(
    Info,
    "Gui::MainChannelContainer::MainChannelContainer()",
    "Instantiated MainChannelContainer"
  );
}

void MainChannelContainer::hydrateState(const AppStatePacket &appState) const {
  mainChannel->hydrateState(appState, 0);
}

void MainChannelContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  std::string styleString = "border-radius: 5px; ";
  styleString += "background-color: " + Color::toHex(GjC::LIGHT_200) + "; ";
  setStyleSheet(styleString.data());
}

void MainChannelContainer::setupGrid() {
  grid.addWidget(mainChannel.get(), 0, 0, -1, -1);
}

void MainChannelContainer::setEffects() const {
  Logging::write(
    Info,
    "Gui::MainChannelContainer::setEffects",
    "Setting effects."
  );
  return mainChannel->setEffects();
}

void MainChannelContainer::setChannel() {
  Logging::write(
    Info,
    "Gui::MainChannelContainer::setChannel",
    "Setting channel."
  );

  mainChannel = std::make_unique<EffectsChannel>(this, actorSystem, mixer, 0, nullptr,
    muteChannelAction, muteLChannelAction, muteRChannelAction,
    soloChannelAction, soloLChannelAction, soloRChannelAction,
    vuPtr
  );

  setupGrid();
  Logging::write(
    Info,
    "Gui::MainChannelContainer::setChannel",
    "Done setting channel."
  );
}

} // Gui
} // Gj