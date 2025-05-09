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
  QAction* soloChannelAction
  )
  : QWidget(parent)
  , actorSystem(actorSystem)
  , appStateManagerPtr(actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER))
  , mixer(mixer)
  , channelIndex(channelIndex)
  , removeEffectsChannelAction(removeEffectsChannelAction)
  , removeEffectsChannelButton(this, channelIndex, removeEffectsChannelAction)
  , effectsContainer(nullptr, mixer, channelIndex)
  , openEffectsContainer(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Open Effects"), this)
  , vstSelect(this)
  , addEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Effect"), this)
  , replaceEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentRevert), tr("&Replace Effect"), this)
  , removeEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::WindowClose), tr("&Remove Effect"), this)
  , grid(this)
  , title(this)
  , gainSlider(Qt::Vertical, this)
  , panSlider(Qt::Horizontal, this)
  , effectsSlots(this, actorSystem, mixer, channelIndex, &addEffectAction, &replaceEffectAction, &removeEffectAction)
  , muteSoloContainer(this, &openEffectsContainer, muteChannelAction, soloChannelAction, channelIndex) {
  if (channelIndex > 1 || mixer->getEffectsChannelsCount() > 1) {
    // can't remove main, must have at least one non-main effects channel
    removeEffectsChannelButton.show();
  } else {
    removeEffectsChannelButton.hide();
  }

  connectActions();

  setupTitle();
  setupGainSlider();
  setupPanSlider();
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

  removeEffectsChannelButton.hydrateState(appState, channelIndex);
  effectsSlots.hydrateState(appState, channelIndex);

  setupTitle();
  setupGrid();
}

void EffectsChannel::updateShowRemoveEffectsChannelButton(bool val) {
  if (val) {
    removeEffectsChannelButton.show();
  } else {
    removeEffectsChannelButton.hide();
  }
}

void EffectsChannel::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: orange;");
}

void EffectsChannel::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, 1);

  // can't delete main, must have at least one non-main channel
  grid.addWidget(&removeEffectsChannelButton, 0, 3, 1, 1);
  grid.addWidget(&gainSlider, 1, 0, -1, -1);
  grid.addWidget(&effectsSlots, 1, 1, 1, 1);
  grid.addWidget(&muteSoloContainer, 2, 1, 1, 1);
  grid.addWidget(&panSlider, 3, 1, 1, 1);

  grid.setVerticalSpacing(2);
  grid.setHorizontalSpacing(2);

  grid.setColumnMinimumWidth(0, 20);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowMinimumHeight(1, 100);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
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

    // TODO:
    // - debug seg fault from rapid messages here
    // - the actor system should be able to handle the amount of messages
    // - but perhaps doesn't like looking in the registry as often
    // - trying to use a stored appStateManagerPtr hasn't worked either, though

    // appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
    // const scoped_actor self{ actorSystem };
    // self->anon_send(
    //     actor_cast<actor>(appStateManagerPtr),
    //     channelIndex,
    //     gainF,
    //     mix_set_channel_gain_a_v
    // );
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

      appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

      const scoped_actor self{ actorSystem };
      self->anon_send(
          actor_cast<actor>(appStateManagerPtr),
          channelIndex,
          effectPath,
          mix_add_effect_to_channel_a_v
      );
      effectsSlots.addEffectSlot();
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

void EffectsChannel::setSolo(const float val) {
  muteSoloContainer.setSolo(val);
}


} // Gui
} // Gj