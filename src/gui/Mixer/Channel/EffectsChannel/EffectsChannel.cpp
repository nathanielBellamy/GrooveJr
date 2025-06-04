//
// Created by ns on 3/7/25.
//

#include "EffectsChannel.h"

namespace Gj {
namespace Gui {

EffectsChannel::EffectsChannel(
  QWidget* parent,
  actor_system& actorSystem,
  Audio::Mixer* mixer,
  const int channelIndex,
  QAction* removeEffectsChannelAction,
  QAction* muteChannelAction,
  QAction* muteLChannelAction,
  QAction* muteRChannelAction,
  QAction* soloChannelAction,
  QAction* soloLChannelAction,
  QAction* soloRChannelAction
  )
  : QWidget(parent)
  , actorSystem(actorSystem)
  , appStateManagerPtr(actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER))
  , mixer(mixer)
  , channelIndex(channelIndex)
  , removeEffectsChannelAction(removeEffectsChannelAction)
  , removeEffectsChannelButton(this, channelIndex, removeEffectsChannelAction)
  , addEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Effect"), this)
  , addEffectButton(this, &addEffectAction)
  , effectsContainer(nullptr, mixer, channelIndex, &addEffectAction)
  , openEffectsContainer(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Open Effects"), this)
  , vstSelect(this)
  , replaceEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentRevert), tr("&Replace Effect"), this)
  , removeEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::WindowClose), tr("&Remove Effect"), this)
  , grid(this)
  , title(this)
  , gainSlider(Qt::Vertical, this)
  , gainLabel("Gain", this)
  , gainLSlider(Qt::Vertical, this)
  , gainLLabel("GainL", this)
  , gainRSlider(Qt::Vertical, this)
  , gainRLabel("GainR", this)
  , panSlider(Qt::Horizontal, this)
  , panLabel("Pan", this)
  , panLSlider(Qt::Horizontal, this)
  , panLLabel("PanL", this)
  , panRSlider(Qt::Horizontal, this)
  , panRLabel("PanR", this)
  , effectsSlots(this, actorSystem, mixer, channelIndex, &replaceEffectAction, &removeEffectAction)
  , effectsSlotsScrollArea(this)
  , muteSoloContainer(
    this, &openEffectsContainer,
    muteChannelAction, muteLChannelAction, muteRChannelAction,
    soloChannelAction, soloLChannelAction, soloRChannelAction,
    channelIndex
  ) {
  if (channelIndex > 1 || mixer->getEffectsChannelsCount() > 1) {
    // can't remove main, must have at least one non-main effects channel
    removeEffectsChannelButton.show();
  } else {
    removeEffectsChannelButton.hide();
  }

  // TODO:
  // - toggle pan controls, pan vs panL/R
  panSlider.hide();
  panLabel.hide();

  connectActions();

  setupTitle();
  setupGainSlider();
  setupGainLSlider();
  setupGainRSlider();
  setupPanSlider();
  setupPanLSlider();
  setupPanRSlider();
  setStyle();
  setupGrid();
}

EffectsChannel::~EffectsChannel() {
  Logging::write(
    Info,
    "Gui::EffectsChannel::~EffectsChannel",
    "Destroying Effects Channel - channelIdx: " + std::to_string(channelIndex)
  );
}

void EffectsChannel::hydrateState(const AppStatePacket& appState, const int newChannelIndex) {
  Logging::write(
    Info,
    "Gui::EffectsChannel::hydrateState",
    "Hydrating effects channel state to channel: " + std::to_string(newChannelIndex)
  );

  channelIndex = newChannelIndex;

  if (appState.playState == PLAY || appState.playState == FF || appState.playState == RW) {
    removeEffectsChannelButton.setEnabled(false);
    addEffectButton.setEnabled(false);
  } else {
    removeEffectsChannelButton.setEnabled(true);
    addEffectButton.setEnabled(true);
  }

  if (channelIndex > 0)
    removeEffectsChannelButton.hydrateState(appState, channelIndex);

  effectsSlots.hydrateState(appState, channelIndex);

  setupTitle();
  // setupGrid();
}

void EffectsChannel::updateShowRemoveEffectsChannelButton(bool val) {
  if (val) {
    removeEffectsChannelButton.show();
  } else {
    removeEffectsChannelButton.hide();
  }
}

void EffectsChannel::setStyle() {
  setFixedHeight(340);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setStyleSheet(
    ("background-color: " + Color::toHex(GjC::LIGHT_300)).data()
  );
}

void EffectsChannel::setupGrid() {
  grid.setVerticalSpacing(4);
  grid.setHorizontalSpacing(4);

  grid.addWidget(&title, 0, 0, 1, 1);

  // can't delete main, must have at least one non-main channel
  grid.addWidget(&removeEffectsChannelButton, 0, 4, 1, 1);
  grid.addWidget(&gainSlider, 1, 0, 3, 1);
  grid.addWidget(&gainLabel, 4, 0, 1, 1);
  grid.addWidget(&gainLSlider, 1, 1, 3, 1);
  grid.addWidget(&gainLLabel, 4, 1, 1, 1);
  grid.addWidget(&gainRSlider, 1, 2, 3, 1);
  grid.addWidget(&gainRLabel, 4, 2, 1, 1);
  grid.addWidget(&effectsSlotsScrollArea, 1, 3, 1, 1);
  grid.addWidget(&addEffectButton, 1, 4, 1, 1);
  // grid.addWidget(&panSlider, 2, 3, 1, 1);
  // grid.addWidget(&panLabel, 2, 4, 1, 1);
  grid.addWidget(&panLSlider, 2, 3, 1, 1);
  grid.addWidget(&panLLabel, 2, 4, 1, 1);
  grid.addWidget(&panRSlider, 3, 3, 1, 1);
  grid.addWidget(&panRLabel, 3, 4, 1, 1);
  grid.addWidget(&muteSoloContainer, 4, 0, 1, -1);

  grid.setVerticalSpacing(2);
  grid.setHorizontalSpacing(2);

  grid.setColumnMinimumWidth(0, 20);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowMinimumHeight(1, 100);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
  setupEffectsSlotsScrollArea();
}

void EffectsChannel::setupEffectsSlotsScrollArea() {
  effectsSlotsScrollArea.setFixedSize(QSize(200, 175));
  effectsSlotsScrollArea.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  effectsSlotsScrollArea.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  effectsSlotsScrollArea.setWidgetResizable(true);
  effectsSlotsScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  effectsSlotsScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  effectsSlotsScrollArea.setLayoutDirection(Qt::LeftToRight);
  effectsSlotsScrollArea.setWidget(&effectsSlots);
  effectsSlots.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void EffectsChannel::setupTitle() {
  if (channelIndex == 0) {
    title.setText("Main");
  } else {
    title.setText("FX " + QString::number(channelIndex));
  }
  title.setFont({title.font().family(), 16});
}

void EffectsChannel::setupGainSlider() {
  gainSlider.setMinimum(0);
  gainSlider.setMaximum(127);
  gainSlider.setTickInterval(1);
  gainSlider.setValue(100);
  gainSlider.setTickPosition(QSlider::NoTicks);
  auto gainSliderConnection = connect(&gainSlider, &QSlider::valueChanged, [this](int gain) {
    const float gainF = static_cast<float>(gain) / 100.0f;

    mixer->getEffectsChannel(channelIndex)->setGain(gainF);
  });
}

void EffectsChannel::setupGainLSlider() {
  gainLSlider.setMinimum(0);
  gainLSlider.setMaximum(127);
  gainLSlider.setTickInterval(1);
  gainLSlider.setValue(100);
  gainLSlider.setTickPosition(QSlider::NoTicks);
  auto gainSliderConnection = connect(&gainLSlider, &QSlider::valueChanged, [this](int gain) {
    const float gainF = static_cast<float>(gain) / 100.0f;

    mixer->getEffectsChannel(channelIndex)->setGainL(gainF);
  });
}

void EffectsChannel::setupGainRSlider() {
  gainRSlider.setMinimum(0);
  gainRSlider.setMaximum(127);
  gainRSlider.setTickInterval(1);
  gainRSlider.setValue(100);
  gainRSlider.setTickPosition(QSlider::NoTicks);
  auto gainSliderConnection = connect(&gainRSlider, &QSlider::valueChanged, [this](int gain) {
    const float gainF = static_cast<float>(gain) / 100.0f;

    mixer->getEffectsChannel(channelIndex)->setGainR(gainF);
  });
}

void EffectsChannel::setupPanSlider() {
  panSlider.setMinimum(-127);
  panSlider.setMaximum(127);
  panSlider.setTickInterval(1);
  panSlider.setValue(0);
  panSlider.setTickPosition(QSlider::NoTicks);
  auto panSliderConnection = connect(&panSlider, &QSlider::valueChanged, [this](const int pan) {
    const float panF = static_cast<float>(pan) / 127.0f;

    mixer->getEffectsChannel(channelIndex)->setPan(panF);
  });
}

void EffectsChannel::setupPanLSlider() {
  panLSlider.setMinimum(-127);
  panLSlider.setMaximum(127);
  panLSlider.setTickInterval(1);
  panLSlider.setValue(-127);
  panLSlider.setTickPosition(QSlider::NoTicks);
  auto panSliderConnection = connect(&panLSlider, &QSlider::valueChanged, [this](const int pan) {
    const float panF = static_cast<float>(pan) / 127.0f;

    mixer->getEffectsChannel(channelIndex)->setPanL(panF);
  });
}

void EffectsChannel::setupPanRSlider() {
  panRSlider.setMinimum(-127);
  panRSlider.setMaximum(127);
  panRSlider.setTickInterval(1);
  panRSlider.setValue(127);
  panRSlider.setTickPosition(QSlider::NoTicks);
  auto panSliderConnection = connect(&panRSlider, &QSlider::valueChanged, [this](const int pan) {
    const float panF = static_cast<float>(pan) / 127.0f;

    mixer->getEffectsChannel(channelIndex)->setPanR(panF);
  });
}

int EffectsChannel::addEffect(const std::string &effectPath) {
  effectsSlots.addEffectSlot();
  const int newEffectIndex = mixer->effectsOnChannelCount(channelIndex) - 1;
  const std::string name = mixer->getPluginName(channelIndex, newEffectIndex);
  effectsContainer.addEffect(newEffectIndex, name);

  return 0;
}


void EffectsChannel::connectActions() {
  auto openEffectsContainerConnection = connect(&openEffectsContainer, &QAction::triggered, [&]() {
    effectsContainer.show();
  });

  auto vstSelectConnection = connect(&vstSelect, &QFileDialog::urlSelected, [&](const QUrl& url) {
    Logging::write(
      Info,
      "Gui::EffectsChannel::vstSelect",
      "Selecting VST for channel " + std::to_string(channelIndex)
    );
    vstUrl = url;
  });

  auto addEffectConnection = connect(&addEffectAction, &QAction::triggered, [&]() {
    if (vstSelect.exec() == QDialog::Accepted) {
      const auto effectPath = vstUrl.toDisplayString().toStdString().substr(7);
      Logging::write(
        Info,
        "Gui::EffectsChannel::addEffectAction",
        "Adding effect: " + effectPath + " to channel " + std::to_string(channelIndex)
      );

      if (!mixer->addEffectToChannel(channelIndex, effectPath)) {
        Logging::write(
          Error,
          "Gui::EffectsChannel::addEffectAction",
          "Unable to add effect " + effectPath + " to channel " + std::to_string(channelIndex)
        );
      } else {
        addEffect(effectPath);

        appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
        const scoped_actor self{ actorSystem };
        self->anon_send(
            actor_cast<actor>(appStateManagerPtr),
            channelIndex,
            effectPath,
            mix_add_effect_to_channel_a_v
        );
      }
    }
  });

  auto replaceEffectConnection = connect(&replaceEffectAction, &QAction::triggered, [&]() {
    const int pluginIdx = replaceEffectAction.data().toInt();
    if (vstSelect.exec() == QDialog::Accepted) {
      const auto effectPath = vstUrl.toDisplayString().toStdString().substr(7);
      Logging::write(
        Info,
        "Gui::EffectsChannel::replaceEffectAction",
        "Replacing effect " + std::to_string(pluginIdx) + " on channel " + std::to_string(channelIndex) + " with " + effectPath
      );

      appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

      const scoped_actor self{ actorSystem };
      self->anon_send(
          actor_cast<actor>(appStateManagerPtr),
          channelIndex,
          pluginIdx,
          effectPath,
          mix_replace_effect_on_channel_a_v
      );
    }
  });

  auto removeEffectConnection = connect(&removeEffectAction, &QAction::triggered, [&]() {
    const int pluginIdx = removeEffectAction.data().toInt();
    Logging::write(
      Info,
      "Gui::EffectsChannel::removeEffectAction",
      "Removing effect: " + std::to_string(pluginIdx) + " from channel " + std::to_string(channelIndex)
    );

    appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    const scoped_actor self{ actorSystem };
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        channelIndex,
        pluginIdx,
        mix_remove_effect_on_channel_a_v
    );
    effectsSlots.removeEffectSlot();
  });
}

void EffectsChannel::setMute(const float val) {
  muteSoloContainer.setMute(val);
}

void EffectsChannel::setMuteL(const float val) {
  muteSoloContainer.setMuteL(val);
}

void EffectsChannel::setMuteR(const float val) {
  muteSoloContainer.setMuteR(val);
}

void EffectsChannel::setSolo(const float val) {
  muteSoloContainer.setSolo(val);
}

void EffectsChannel::setSoloL(const float val) {
  muteSoloContainer.setSoloL(val);
}

void EffectsChannel::setSoloR(const float val) {
  muteSoloContainer.setSoloR(val);
}

void EffectsChannel::reset() {
  // TODO
}

} // Gui
} // Gj