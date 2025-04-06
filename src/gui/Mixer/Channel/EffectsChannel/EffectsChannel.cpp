//
// Created by ns on 3/7/25.
//

#include "EffectsChannel.h"

namespace Gj {
namespace Gui {

EffectsChannel::EffectsChannel(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer, const int channelIndex)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , effectsContainer(nullptr, mixer, channelIndex)
  , openEffectsContainer(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Open Effects"), this)
  , vstSelect(this)
  , addEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Effect"), this)
  , grid(this)
  , title(this)
  , slider(Qt::Vertical, this)
  , effectsSlots(this, actorSystem, mixer, channelIndex, &addEffectAction)
  , muteSoloContainer(this, &openEffectsContainer)
  {

  connectActions();

  setupTitle();
  setupSlider();
  setStyle();
  setupGrid();
}

void EffectsChannel::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: orange;");
}

void EffectsChannel::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, -1);
  grid.addWidget(&slider, 1, 0, -1, -1);
  grid.addWidget(&effectsSlots, 1, 1, 1, 1);
  grid.addWidget(&muteSoloContainer, 2, 1, 1, 1);

  grid.setVerticalSpacing(2);
  grid.setHorizontalSpacing(2);

  grid.setColumnMinimumWidth(0, 20);
  grid.setRowStretch(0, 1);
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
    std::cout << "selecting vst for channel " << channelIndex << std::endl;
    Logging::write(
      Info,
      "EffectsChannel::vstSelect",
      "Selecting VST for channel " + std::to_string(channelIndex)
    );
    vstUrl = url;
  });

  connect(&addEffectAction, &QAction::triggered, [&]() {
    if (vstSelect.exec() == QDialog::Accepted) {
      Logging::write(
        Info,
        "EffectsChannel::addEffectAction",
        "Adding effect: " + vstUrl.toDisplayString().toStdString() + " to channel " + std::to_string(channelIndex)
      );
      if (!mixer->addEffectToChannel(channelIndex, vstUrl.toDisplayString().toStdString().substr(7)))
        Logging::write(
          Error,
          "EffectsChannel::addEffectAction",
          "Unable to add effect: " + vstUrl.toDisplayString().toStdString() + " to channel " + std::to_string(channelIndex)
        );
    }
  });
}

} // Gui
} // Gj