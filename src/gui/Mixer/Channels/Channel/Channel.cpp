//
// Created by ns on 3/7/25.
//

#include "Channel.h"


namespace Gj {
namespace Gui {
namespace Mixer {
Channel::Channel(
  QWidget* parent,
  actor_system& actorSystem,
  Audio::Mixer::Core* mixer,
  const ChannelIndex channelIndex,
  QAction* removeChannelAction,
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
  , removeChannelAction(removeChannelAction)
  , removeChannelButton(this, channelIndex, removeChannelAction)
  , addPluginAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Plugin"), this)
  , addPluginButton(this, &addPluginAction)
  , openPluginsContainer(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Open Plugins"), this)
  , vstSelect(this)
  , replacePluginAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentRevert), tr("&Replace Plugin"), this)
  , removePluginAction(QIcon::fromTheme(QIcon::ThemeIcon::WindowClose), tr("&Remove Plugin"), this)
  , pluginsContainer(nullptr, mixer, channelIndex, &addPluginAction, &removePluginAction)
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
  , pluginSlotsScrollArea(this)
  , pluginSlots(this, actorSystem, mixer, channelIndex, &replacePluginAction, &removePluginAction)
  , muteSoloContainer(
    this, mixer, channelIndex, &openPluginsContainer,
    muteChannelAction, muteLChannelAction, muteRChannelAction,
    soloChannelAction, soloLChannelAction, soloRChannelAction
  ) {
  if (channelIndex > 1 || mixer->getChannelsCount() > 1) {
    // can't remove main, must have at least one non-main plugins channel
    removeChannelButton.show();
  } else {
    removeChannelButton.hide();
  }

  // TODO:
  // - toggle pan controls, pan vs panL/R
  panSlider.hide();
  panLabel.hide();

  connectActions();
  setupTitle();


  const auto res = mixer->runAgainstChannel(channelIndex, [this](const Audio::Mixer::Channel* channel) {
    setupGainSlider(channel->settings.gain.load());
    setupGainLSlider(channel->settings.gainL.load());
    setupGainRSlider(channel->settings.gainR.load());
    setupPanSlider(channel->settings.pan.load());
    setupPanLSlider(channel->settings.panL.load());
    setupPanRSlider(channel->settings.panR.load());
  });

  if (res != OK)
    Logging::write(
      res == WARNING ? Warning : Error,
      "Gui::Channel::PluginsChannel()",
      "An Error occurred during construction against mixer channelIndex: " + std::to_string(channelIndex)
    );

  setStyle();
  setupGrid();

  Logging::write(
    Info,
    "Gui::Channel::PluginsChannel()",
    "Instantiated Channel channelIdx: " + std::to_string(channelIndex)
  );
}

Channel::~Channel() {
  Logging::write(
    Info,
    "Gui::Channel::~PluginsChannel",
    "Destroying Plugins Channel - channelIdx: " + std::to_string(channelIndex)
  );
}

void Channel::hydrateState(const State::Packet& statePacket, const ChannelIndex newChannelIndex) {
  Logging::write(
    Info,
    "Gui::Channel::hydrateState",
    "Hydrating plugins channel state to channel: " + std::to_string(newChannelIndex)
  );

  channelIndex = newChannelIndex;

  if (channelIndex > 0)
    removeChannelButton.hydrateState(statePacket, channelIndex);

  pluginSlots.hydrateState(statePacket, channelIndex);
  vuMeter.hydrateState(statePacket);

  setupTitle();
  // setupGrid();
}

void Channel::updateShowRemoveChannelButton(const bool val) {
  if (val)
    removeChannelButton.show();
  else
    removeChannelButton.hide();
}

void Channel::setStyle() {
  setFixedHeight(340);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setStyleSheet(
    ("background-color: " + Color::toHex(GjC::LIGHT_300)).data()
  );
}

void Channel::setupGrid() {
  grid.setVerticalSpacing(4);
  grid.setHorizontalSpacing(4);

  grid.addWidget(&title, 0, 0, 1, 1);

  // can't delete main, must have at least one non-main channel
  grid.addWidget(&removeChannelButton, 0, 4, 1, 1);
  grid.addWidget(&vuMeter, 1, 0, 3, 1);
  grid.addWidget(&gainSlider, 1, 1, 3, 1);
  grid.addWidget(&gainLabel, 4, 1, 1, 1);
  grid.addWidget(&gainLSlider, 1, 2, 3, 1);
  grid.addWidget(&gainLLabel, 4, 2, 1, 1);
  grid.addWidget(&gainRSlider, 1, 3, 3, 1);
  grid.addWidget(&gainRLabel, 4, 3, 1, 1);
  grid.addWidget(&pluginSlotsScrollArea, 1, 4, 1, 1);
  grid.addWidget(&addPluginButton, 1, 5, 1, 1);
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
  setupPluginSlotsScrollArea();
}

void Channel::setupPluginSlotsScrollArea() {
  pluginSlotsScrollArea.setFixedSize(QSize(200, 175));
  pluginSlotsScrollArea.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  pluginSlotsScrollArea.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  pluginSlotsScrollArea.setWidgetResizable(true);
  pluginSlotsScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  pluginSlotsScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  pluginSlotsScrollArea.setLayoutDirection(Qt::LeftToRight);
  pluginSlotsScrollArea.setWidget(&pluginSlots);
  pluginSlots.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void Channel::setupTitle() {
  if (channelIndex == 0) {
    title.setText("Main");
  } else {
    title.setText("FX " + QString::number(channelIndex));
  }
  title.setFont({title.font().family(), 16});
}

constexpr float GAIN_FACTOR = 2.0f;

void Channel::setupGainSlider(const float gain) {
  gainSlider.setMinimum(0);
  gainSlider.setMaximum(127);
  gainSlider.setTickInterval(1);
  gainSlider.setValue(
    Audio::Math::floatToUInt127(gain)
  );
  gainSlider.setTickPosition(QSlider::NoTicks);
  auto gainSliderConnection = connect(&gainSlider, &QSlider::valueChanged, [this](const int newGain) {
    const auto res = mixer->runAgainstChannel(channelIndex, [this, &newGain](Audio::Mixer::Channel* channel) {
      channel->setGain(
        Audio::Math::uInt127ToFloat(newGain) * GAIN_FACTOR
      );
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Channel::setupGainSlider",
        "An Error occurred setting gain slider against mixer channelIndex: " + std::to_string(channelIndex)
      );
  });
}

void Channel::setupGainLSlider(const float gainL) {
  gainLSlider.setMinimum(0);
  gainLSlider.setMaximum(127);
  gainLSlider.setTickInterval(1);
  gainLSlider.setValue(
    Audio::Math::floatToInt127(gainL)
  );
  gainLSlider.setTickPosition(QSlider::NoTicks);
  auto gainSliderConnection = connect(&gainLSlider, &QSlider::valueChanged, [this](const int newGainL) {
    const auto res = mixer->runAgainstChannel(channelIndex, [this, &newGainL](Audio::Mixer::Channel* channel) {
      channel->setGainL(
        Audio::Math::uInt127ToFloat(newGainL) * GAIN_FACTOR
      );
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Channel::setupGainLSlider",
        "An Error occurred setting gainL slider against mixer channelIndex: " + std::to_string(channelIndex)
      );
  });
}

void Channel::setupGainRSlider(const float gainR) {
  gainRSlider.setMinimum(0);
  gainRSlider.setMaximum(127);
  gainRSlider.setTickInterval(1);
  gainRSlider.setValue(
    Audio::Math::floatToUInt127(gainR)
  );
  gainRSlider.setTickPosition(QSlider::NoTicks);
  const auto gainRSliderConnection = connect(&gainRSlider, &QSlider::valueChanged, [this](const int newGainR) {
    const auto res = mixer->runAgainstChannel(channelIndex, [this, &newGainR](Audio::Mixer::Channel* channel) {
      channel->setGainR(
        Audio::Math::uInt127ToFloat(newGainR) * GAIN_FACTOR
      );
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Channel::setupGainRSlider",
        "An Error occurred setting gainR slider against mixer channelIndex: " + std::to_string(channelIndex)
      );
  });
}

void Channel::setupPanSlider(const float pan) {
  panSlider.setMinimum(-127);
  panSlider.setMaximum(127);
  panSlider.setTickInterval(1);
  panSlider.setValue(
    Audio::Math::floatToInt127(pan)
  );
  panSlider.setTickPosition(QSlider::NoTicks);
  auto panSliderConnection = connect(&panSlider, &QSlider::valueChanged, [this](const int newPan) {
    const auto res = mixer->runAgainstChannel(channelIndex, [this, &newPan](Audio::Mixer::Channel* channel) {
      channel->setPan(
        Audio::Math::int127ToFloat(newPan)
      );
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Channel::setupPanSlider",
        "An Error occurred setting pan slider against mixer channelIndex: " + std::to_string(channelIndex)
      );
  });
}

void Channel::setupPanLSlider(const float panL) {
  panLSlider.setMinimum(-100);
  panLSlider.setMaximum(100);
  panLSlider.setTickInterval(1);
  panLSlider.setValue(
    Audio::Math::floatToInt127(panL)
  );
  panLSlider.setTickPosition(QSlider::NoTicks);
  auto panSliderConnection = connect(&panLSlider, &QSlider::valueChanged, [this](const int newPanL) {
    const auto res = mixer->runAgainstChannel(channelIndex, [this, &newPanL](Audio::Mixer::Channel* channel) {
      channel->setPanL(
        Audio::Math::int127ToFloat(newPanL)
      );
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Channel::setupPanLSlider",
        "An Error occurred setting panL slider against mixer channelIndex: " + std::to_string(channelIndex)
      );
  });
}

void Channel::setupPanRSlider(const float panR) {
  panRSlider.setMinimum(-100);
  panRSlider.setMaximum(100);
  panRSlider.setTickInterval(1);
  panRSlider.setValue(
    Audio::Math::floatToInt127(panR)
  );
  panRSlider.setTickPosition(QSlider::NoTicks);
  auto panSliderConnection = connect(&panRSlider, &QSlider::valueChanged, [this](const int newPanR) {
    const auto res = mixer->runAgainstChannel(channelIndex, [this, &newPanR](Audio::Mixer::Channel* channel) {
      channel->setPanR(
        Audio::Math::int127ToFloat(newPanR)
      );
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Channel::setupPanRSlider",
        "An Error occurred setting panR slider against mixer channelIndex: " + std::to_string(channelIndex)
      );
  });
}

void Channel::setPlugins() {
  Logging::write(
    Info,
    "Gui::Channel::setPlugins",
    "Setting plugins on channelIndex: " + std::to_string(channelIndex)
  );

  pluginSlots.reset();
  if (channelIndex > mixer->getChannelsCount())
    return;

  for (PluginIndex i = 0; i < mixer->getPluginsOnChannelCount(channelIndex); i++)
    addPlugin(std::optional(i));

  Logging::write(
    Info,
    "Gui::Channel::setPlugins",
    "Done setting plugins on channelIndex: " + std::to_string(channelIndex)
  );
}

void Channel::addPlugin(const std::optional<PluginIndex> pluginIndex) {
  const PluginIndex newPluginIndex = pluginIndex.value_or(
    mixer->getPluginsOnChannelCount(channelIndex) - 1
  );
  Logging::write(
    Info,
    "Gui::Channel::addPlugin",
    "Adding plugin at index: " + std::to_string(newPluginIndex)
  );

  const AtomicStr name = mixer->getPluginName(channelIndex, newPluginIndex);
  pluginsContainer.addPlugin(newPluginIndex, name);

  Logging::write(
    Info,
    "Gui::Channel::addPlugin",
    "Added plugin " + name + " at index: " + std::to_string(newPluginIndex)
  );
}

void Channel::connectActions() {
  auto openPluginsContainerConnection = connect(&openPluginsContainer, &QAction::triggered, [&]() {
    pluginsContainer.show();
  });

  auto vstSelectConnection = connect(&vstSelect, &QFileDialog::urlSelected, [&](const QUrl& url) {
    Logging::write(
      Info,
      "Gui::Channel::vstSelect",
      "Selecting VST for channel " + std::to_string(channelIndex)
    );
    vstUrl = url;
  });

  auto addPluginConnection = connect(&addPluginAction, &QAction::triggered, [&]() {
    if (vstSelect.exec() == QDialog::Accepted) {
      const PluginPath pluginPath = vstUrl.toDisplayString().toStdString().substr(7);
      Logging::write(
        Info,
        "Gui::Channel::addPluginAction",
        "Adding plugin: " + pluginPath + " to channel " + std::to_string(channelIndex)
      );

      appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
      const scoped_actor self{actorSystem};
      self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        channelIndex,
        pluginPath,
        mix_add_plugin_to_channel_a_v
      );
    }
  });

  auto replacePluginConnection = connect(&replacePluginAction, &QAction::triggered, [&]() {
    const PluginIndex pluginIdx = replacePluginAction.data().toULongLong();
    if (vstSelect.exec() == QDialog::Accepted) {
      const auto pluginPath = vstUrl.toDisplayString().toStdString().substr(7);
      Logging::write(
        Info,
        "Gui::Channel::replacePluginAction",
        "Replacing plugin " + std::to_string(pluginIdx) + " on channel " + std::to_string(channelIndex) + " with " +
        pluginPath
      );

      appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
      const scoped_actor self{actorSystem};
      self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        channelIndex,
        pluginIdx,
        pluginPath,
        mix_replace_plugin_on_channel_a_v
      );
    }
  });

  auto removePluginConnection = connect(&removePluginAction, &QAction::triggered, [&]() {
    const PluginIndex pluginIdx = removePluginAction.data().toULongLong();
    Logging::write(
      Info,
      "Gui::Channel::removePluginAction",
      "Removing plugin: " + std::to_string(pluginIdx) + " from channel " + std::to_string(channelIndex)
    );

    appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
    const scoped_actor self{actorSystem};
    self->anon_send(
      actor_cast<actor>(appStateManagerPtr),
      channelIndex,
      pluginIdx,
      mix_remove_plugin_on_channel_a_v
    );
  });
}

void Channel::setMute(const float val) {
  muteSoloContainer.setMute(val);
}

void Channel::setMuteL(const float val) {
  muteSoloContainer.setMuteL(val);
}

void Channel::setMuteR(const float val) {
  muteSoloContainer.setMuteR(val);
}

void Channel::setSolo(const float val) {
  muteSoloContainer.setSolo(val);
}

void Channel::setSoloL(const float val) {
  muteSoloContainer.setSoloL(val);
}

void Channel::setSoloR(const float val) {
  muteSoloContainer.setSoloR(val);
}
} // Mixer
} // Gui
} // Gj
