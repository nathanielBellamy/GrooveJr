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
#include "../enums/Result.h"

#include "../Logging.h"

#include "./Color.h"
#include "./MenuBar/MenuBar.h"
#include "./Mixer/Body.h"
#include "./MusicLibrary/MusicLibraryWindow.h"
#include "./MainToolBar/MainToolBar.h"

#include "../state/AppState.h"
#include "../audio/mixer/Core.h"
#include "Hydrater.h"

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QGridLayout>
#include <QThread>
#include "QSql/SqlWorkerPoolHost.h"

using namespace caf;

namespace Gj {
namespace Gui {
class MainWindow final : public SqlWorkerPoolHost {
  Q_OBJECT

public:
  explicit MainWindow(
    actor_system& actorSystem,
    Audio::Mixer::Core* mixer,
    AppState* gAppState,
    void (*shutdown_handler)(int)
  );

  ~MainWindow();

  void closeEvent(QCloseEvent* event) override;

  void setChannels();

  void setPlugins();

  void connectHydrater(const Hydrater& hydrater) {
    const auto hydraterConnection =
        connect(&hydrater, &Hydrater::hydrate, this, [&](const AppStatePacket& packet) {
          hydrateState(packet);
        });
  };

public slots:
  Result hydrateState(const AppStatePacket& appStatePacket);

private:
  AppState* gAppState;
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;

  void (*shutdown_handler)(int);

  QThread sqlWorkerPoolThread;
  SqlWorkerPool* sqlWorkerPool;
  QWidget container;
  MenuBar* menuBar;
  QAction sceneLoadAction;
  MainToolBar mainToolBar;
  QGridLayout grid;
  MusicLibraryWindow musicLibraryWindow;
  Mixer::Body mixerWindow;

  void setupGrid();

  void connectActions();

  SqlWorkerPool* initQSql();
};
} // Gui
} // Gj


#endif //MAINWINDOW_H
