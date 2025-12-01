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
  const ChannelIndex channelIndex,
  QAction* removeEffectsChannelAction,
  QAction* muteChannelAction,
  QAction* muteLChannelAction,
  QAction* muteRChannelAction,
  QAction* soloChannelAction,
  QAction* soloLChannelAction,
  QAction* soloRChannelAction,
  std::atomic<float>* vuPtr
  )
  : QWidget(parent)
  , channelIndex(channelIndex)
  , actorSystem(actorSystem)
  , appStateManagerPtr(actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER))
  , mixer(mixer)
  , vuPtr(vuPtr)
  , vuMeter(this, mixer, vuPtr, channelIndex)
  , removeEffectsChannelAction(removeEffectsChannelAction)
  , removeEffectsChannelButton(this, channelIndex, removeEffectsChannelAction)
  , addEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Effect"), this)
  , addEffectButton(this, &addEffectAction)
  , openEffectsContainer(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Open Effects"), this)
  , vstSelect(this)
  , replaceEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentRevert), tr("&Replace Effect"), this)
  , removeEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::WindowClose), tr("&Remove Effect"), this)
  , effectsContainer(nullptr, mixer, channelIndex, &addEffectAction, &removeEffectAction)
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
  , effectsSlotsScrollArea(this)
  , effectsSlots(this, actorSystem, mixer, channelIndex, &replaceEffectAction, &removeEffectAction)
  , muteSoloContainer(
    this, mixer, channelIndex, &openEffectsContainer,
    muteChannelAction, muteLChannelAction, muteRChannelAction,
    soloChannelAction, soloLChannelAction, soloRChannelAction
  )
  {
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

  const auto& channel = mixer->getEffectsChannel(channelIndex)->channel;

  setupGainSlider(channel.gain.load());
  setupGainLSlider(channel.gainL.load());
  setupGainRSlider(channel.gainR.load());
  setupPanSlider(channel.pan.load());
  setupPanLSlider(channel.panL.load());
  setupPanRSlider(channel.panR.load());
  setStyle();
  setupGrid();

  Logging::write(
    Info,
    "Gui::EffectsChannel::EffectsChannel()",
    "Instantiated EffectsChannel channelIdx: " + std::to_string(channelIndex)
  );
}

EffectsChannel::~EffectsChannel() {
  Logging::write(
    Info,
    "Gui::EffectsChannel::~EffectsChannel",
    "Destroying Effects Channel - channelIdx: " + std::to_string(channelIndex)
  );
}

void EffectsChannel::hydrateState(const AppStatePacket& appStatePacket, const ChannelIndex newChannelIndex) {
  Logging::write(
    Info,
    "Gui::EffectsChannel::hydrateState",
    "Hydrating effects channel state to channel: " + std::to_string(newChannelIndex)
  );

  channelIndex = newChannelIndex;

  if (appStatePacket.playState == PLAY || appStatePacket.playState == FF || appStatePacket.playState == RW) {
    removeEffectsChannelButton.setEnabled(false);
    addEffectButton.setEnabled(false);
  } else {
    removeEffectsChannelButton.setEnabled(true);
    addEffectButton.setEnabled(true);
  }

  if (channelIndex > 0)
    removeEffectsChannelButton.hydrateState(appStatePacket, channelIndex);

  effectsSlots.hydrateState(appStatePacket, channelIndex);
  vuMeter.hydrateState(appStatePacket);

  setupTitle();
  // setupGrid();
}

void EffectsChannel::updateShowRemoveEffectsChannelButton(const bool val) {
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
  grid.addWidget(&vuMeter, 1, 0, 3, 1);
  grid.addWidget(&gainSlider, 1, 1, 3, 1);
  grid.addWidget(&gainLabel, 4, 1, 1, 1);
  grid.addWidget(&gainLSlider, 1, 2, 3, 1);
  grid.addWidget(&gainLLabel, 4, 2, 1, 1);
  grid.addWidget(&gainRSlider, 1, 3, 3, 1);
  grid.addWidget(&gainRLabel, 4, 3, 1, 1);
  grid.addWidget(&effectsSlotsScrollArea, 1, 4, 1, 1);
  grid.addWidget(&addEffectButton, 1, 5, 1, 1);
  // grid.addWidget(&panSlider, 2, 3, 1, 1);
  // grid.addWidget(&panLabel, 2, 4, 1, 1);
  grid.addWidget(&panLSlider, 2, 4, 1, 1);
  grid.addWidget(&panLLabel, 2, 5, 1, 1);
  grid.addWidget(&panRSlider, 3, 4, 1, 1);
  grid.addWidget(&panRLabel, 3, 5, 1, 1);
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

constexpr float gainFactor = 2.0f;
void EffectsChannel::setupGainSlider(const float gain) {
  gainSlider.setMinimum(0);
  gainSlider.setMaximum(127);
  gainSlider.setTickInterval(1);
  gainSlider.setValue(
    Audio::Math::floatToUInt127(gain)
  );
  gainSlider.setTickPosition(QSlider::NoTicks);
  auto gainSliderConnection = connect(&gainSlider, &QSlider::valueChanged, [this](const int newGain) {
    mixer->getEffectsChannel(channelIndex)->setGain(
      Audio::Math::uInt127ToFloat(newGain) * gainFactor
    );
  });
}

void EffectsChannel::setupGainLSlider(const float gainL) {
  gainLSlider.setMinimum(0);
  gainLSlider.setMaximum(127);
  gainLSlider.setTickInterval(1);
  gainLSlider.setValue(
    Audio::Math::floatToInt127(gainL)
  );
  gainLSlider.setTickPosition(QSlider::NoTicks);
  auto gainSliderConnection = connect(&gainLSlider, &QSlider::valueChanged, [this](const int newGainL) {
    mixer->getEffectsChannel(channelIndex)->setGainL(
      Audio::Math::uInt127ToFloat(newGainL) * gainFactor
    );
  });
}

void EffectsChannel::setupGainRSlider(const float gainR) {
  gainRSlider.setMinimum(0);
  gainRSlider.setMaximum(127);
  gainRSlider.setTickInterval(1);
  gainRSlider.setValue(
    Audio::Math::floatToUInt127(gainR)
  );
  gainRSlider.setTickPosition(QSlider::NoTicks);
  auto gainSliderConnection = connect(&gainRSlider, &QSlider::valueChanged, [this](const int newGainR) {
    mixer->getEffectsChannel(channelIndex)->setGainR(
      Audio::Math::uInt127ToFloat(newGainR) * gainFactor
    );
  });
}

void EffectsChannel::setupPanSlider(const float pan) {
  panSlider.setMinimum(-127);
  panSlider.setMaximum(127);
  panSlider.setTickInterval(1);
  panSlider.setValue(
    Audio::Math::floatToInt127(pan)
  );
  panSlider.setTickPosition(QSlider::NoTicks);
  auto panSliderConnection = connect(&panSlider, &QSlider::valueChanged, [this](const int newPan) {
    mixer->getEffectsChannel(channelIndex)->setPan(
      Audio::Math::int127ToFloat(newPan)
    );
  });
}

void EffectsChannel::setupPanLSlider(const float panL) {
  panLSlider.setMinimum(-100);
  panLSlider.setMaximum(100);
  panLSlider.setTickInterval(1);
  panLSlider.setValue(
    Audio::Math::floatToInt127(panL)
  );
  panLSlider.setTickPosition(QSlider::NoTicks);
  auto panSliderConnection = connect(&panLSlider, &QSlider::valueChanged, [this](const int newPanL) {
    mixer->getEffectsChannel(channelIndex)->setPanL(
      Audio::Math::int127ToFloat(newPanL)
    );
  });
}

void EffectsChannel::setupPanRSlider(const float panR) {
  panRSlider.setMinimum(-100);
  panRSlider.setMaximum(100);
  panRSlider.setTickInterval(1);
  panRSlider.setValue(
    Audio::Math::floatToInt127(panR)
  );
  panRSlider.setTickPosition(QSlider::NoTicks);
  auto panSliderConnection = connect(&panRSlider, &QSlider::valueChanged, [this](const int newPanR) {
    mixer->getEffectsChannel(channelIndex)->setPanR(
      Audio::Math::int127ToFloat(newPanR)
    );
  });
}

void EffectsChannel::setEffects() {
  Logging::write(
    Info,
    "Gui::EffectsChannel::setEffects",
    "Setting effects on channelIndex: " + std::to_string(channelIndex)
  );

  effectsSlots.reset();
  if (channelIndex > mixer->getEffectsChannelsCount())
    return;

  for (int i = 0; i < mixer->getEffectsChannel(channelIndex)->effectCount(); i++)
    addEffect(i);

  Logging::write(
    Info,
    "Gui::EffectsChannel::setEffects",
    "Done setting effects on channelIndex: " + std::to_string(channelIndex)
  );
}

void EffectsChannel::addEffect(const std::optional<PluginIndex> effectIndex) {
  const PluginIndex newEffectIndex = effectIndex.value_or(
    mixer->effectsOnChannelCount(channelIndex) - 1
  );
  Logging::write(
    Info,
    "Gui::EffectsChannel::addEffect",
    "Adding effect at index: " + std::to_string(newEffectIndex)
  );

  effectsSlots.addEffectSlot();
  const AtomicStr name = mixer->getPluginName(channelIndex, newEffectIndex);
  effectsContainer.addEffect(newEffectIndex, name);

  Logging::write(
    Info,
    "Gui::EffectsChannel::addEffect",
    "Added effect " + name + " at index: " + std::to_string(newEffectIndex)
  );
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
        addEffect(std::optional<PluginIndex>());

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
    const PluginIndex pluginIdx = replaceEffectAction.data().toULongLong();
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
    const PluginIndex pluginIdx = removeEffectAction.data().toULongLong();
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

} // Gui
} // Gj