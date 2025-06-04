//
// Created by ns on 11/4/24.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <format>

#include "caf/actor_system.hpp"

#include "../actors/ActorIds.h"
#include "../messaging/atoms.h"
#include "../messaging/EnvelopeQtPtr.h"

#include "../Logging.h"

#include "./Color.h"
#include "./MenuBar/MenuBar.h"
#include "./Mixer/MixerWindow.h"
#include "./MusicLibrary/MusicLibraryWindow.h"
#include "./MainToolBar/MainToolBar.h"

#include "../audio/Mixer.h"

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QGridLayout>

using namespace caf;

namespace Gj {
namespace Gui {

class MainWindow final : public QMainWindow {
  public:
    explicit MainWindow(actor_system& actorSystem, Audio::Mixer* mixer, void (*shutdown_handler) (int));
    int hydrateState(const AppStatePacket& appStatePacket);
    void closeEvent(QCloseEvent* event) override;
    void setEffects();

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    void (*shutdown_handler)(int);
    QWidget container;
    MenuBar* menuBar;
    QAction sceneLoadAction;
    MainToolBar mainToolBar;
    QGridLayout grid;
    MusicLibraryWindow musicLibraryWindow;
    MixerWindow mixerWindow;

    void resetChannels(int effectsChannelsCount);
    void setupGrid();
    void connectActions();
};

} // Gui
} // Gj



#endif //MAINWINDOW_H
