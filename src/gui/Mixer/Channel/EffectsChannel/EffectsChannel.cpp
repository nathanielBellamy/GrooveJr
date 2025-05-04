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
  QAction* removeEffectsChannelAction
  )
  : QWidget(parent)
  , actorSystem(actorSystem)
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
  , slider(Qt::Vertical, this)
  , effectsSlots(this, actorSystem, mixer, channelIndex, &addEffectAction, &replaceEffectAction, &removeEffectAction)
  , muteSoloContainer(this, &openEffectsContainer, channelIndex)
  {

  if (channelIndex > 0 && mixer->getEffectsChannelsCount() > 1) {
    // can't remove main, must have at least one non-main effects channel
    removeEffectsChannelButton.show();
  } else {
    removeEffectsChannelButton.hide();
  }

  connectActions();

  setupTitle();
  setupSlider();
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

void EffectsChannel::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: orange;");
}

void EffectsChannel::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, 1);

  // can't delete main, must have at least one non-main channel
  grid.addWidget(&removeEffectsChannelButton, 0, 3, 1, 1);
  grid.addWidget(&slider, 1, 0, -1, -1);
  grid.addWidget(&effectsSlots, 1, 1, 1, 1);
  grid.addWidget(&muteSoloContainer, 2, 1, 1, 1);

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

void EffectsChannel::setupSlider() {
  slider.setMinimum(0);
  slider.setMaximum(127);
  slider.setTickInterval(1);
  slider.setValue(63);
  slider.setTickPosition(QSlider::NoTicks);
}

void EffectsChannel::connectActions() {
  connect(&openEffectsContainer, &QAction::triggered, [&]() {
    effectsContainer.show();
  });

  connect(&vstSelect, &QFileDialog::urlSelected, [&](const QUrl& url) {
    Logging::write(
      Info,
      "Gui::EffectsChannel::vstSelect",
      "Selecting VST for channel " + std::to_string(channelIndex)
    );
    vstUrl = url;
  });

  connect(&addEffectAction, &QAction::triggered, [&]() {
    if (vstSelect.exec() == QDialog::Accepted) {
      const auto effectPath = vstUrl.toDisplayString().toStdString().substr(7);
      Logging::write(
        Info,
        "Gui::EffectsChannel::addEffectAction",
        "Adding effect: " + effectPath + " to channel " + std::to_string(channelIndex)
      );

      strong_actor_ptr appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

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

  connect(&replaceEffectAction, &QAction::triggered, [&]() {
    const int pluginIdx = replaceEffectAction.data().toInt();
    if (vstSelect.exec() == QDialog::Accepted) {
      const auto effectPath = vstUrl.toDisplayString().toStdString().substr(7);
      Logging::write(
        Info,
        "Gui::EffectsChannel::replaceEffectAction",
        "Replacing effect " + std::to_string(pluginIdx) + " on channel " + std::to_string(channelIndex) + " with " + effectPath
      );

      strong_actor_ptr appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

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

  connect(&removeEffectAction, &QAction::triggered, [&]() {
    const int pluginIdx = removeEffectAction.data().toInt();
    Logging::write(
      Info,
      "Gui::EffectsChannel::removeEffectAction",
      "Removing effect: " + std::to_string(pluginIdx) + " from channel " + std::to_string(channelIndex)
    );

    strong_actor_ptr appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

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

} // Gui
} // Gj