//
// Created by ns on 11/4/24.
//

#include "./MainWindow.h"

#include "../main.h"

using namespace caf;

namespace Gj {
namespace Gui {
MainWindow::MainWindow(actor_system& actorSystem, Audio::Mixer::Core* mixer, AppState* gAppState,
                       void (*shutdown_handler)(int))
: SqlWorkerPoolHost(nullptr)
  , gAppState(gAppState)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , shutdown_handler(shutdown_handler)
  , sqlWorkerPool(initQSql())
  , container(this)
  , menuBar(new MenuBar(actorSystem, this))
  , sceneLoadAction(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen), tr("&Select Scene"), this)
  , mainToolBar(this, actorSystem, gAppState, mixer, sqlWorkerPool, &sceneLoadAction)
  , grid(&container)
  , musicLibraryWindow(&container, actorSystem, gAppState, mixer->dao, sqlWorkerPool)
  , mixerWindow(&container, actorSystem, mixer) {
  Logging::write(
    Info,
    "Gui::MainWindow::MainWindow()",
    "Initialized QSql."
  );

  container.setMinimumSize(QSize(1300, 700));
  setCentralWidget(&container);
  setupGrid();
  setStyleSheet(
    "font-weight: 900;"
  );
  connectActions();
  addToolBar(Qt::TopToolBarArea, &mainToolBar);
  setUnifiedTitleAndToolBarOnMac(true);
  setWindowTitle("GrooveJr");

  Logging::write(
    Info,
    "Gui::MainWindow::MainWindow()",
    "Instantiated MainWindow"
  );
}

MainWindow::~MainWindow() {
  sqlWorkerPoolThread.quit();
  delete sqlWorkerPool;
}

SqlWorkerPool* MainWindow::initQSql() {
  const auto sqlWorkerPool = new SqlWorkerPool(this);
  sqlWorkerPool->moveToThread(&sqlWorkerPoolThread);
  sqlWorkerPoolThread.start();
  emit initSqlWorkerPool();
  return sqlWorkerPool;
}

Result MainWindow::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
    Info,
    "Gui::MainWindow::hydrateState",
    "Hydrating app state to Gui: id: " + std::to_string(appStatePacket.id) + " sceneId: " + std::to_string(
      appStatePacket.sceneId)
  );

  mainToolBar.hydrateState(appStatePacket);
  mixerWindow.hydrateState(appStatePacket);
  musicLibraryWindow.hydrateState(appStatePacket);

  Logging::write(
    Info,
    "Gui::MainWindow::hydrateState",
    "Done hydrating state."
  );

  return OK;
}

void MainWindow::setupGrid() {
  grid.setVerticalSpacing(1);
  grid.setColumnStretch(0, 1);

  grid.addWidget(&musicLibraryWindow, 0, 0, 1, -1);
  grid.addWidget(&mixerWindow, 1, 0, -1, -1);
  for (int i = 0; i < 2; i++) {
    grid.setRowMinimumHeight(i, 200);
    grid.setRowStretch(i, 1);
  }

  container.setLayout(&grid);
  std::string styleString = "background-color: " + Color::toHex(GjC::DARK_400) + "; ";
  container.setStyleSheet(styleString.data());
}

void MainWindow::closeEvent(QCloseEvent* e) {
  Logging::write(
    Info,
    "Gui::MainWindow::closeEvent",
    "Closing GroveJr"
  );
  shutdown_handler(3);
}

void MainWindow::setChannels() {
  Logging::write(
    Info,
    "Gui::MainWindow::setChannels",
    "Setting channels."
  );

  mixerWindow.setChannels();

  Logging::write(
    Info,
    "Gui::MainWindow::setChannels",
    "Done setting channels."
  );
}

void MainWindow::setEffects() {
  Logging::write(
    Info,
    "Gui::MainWindow::setEffects",
    "Setting effects."
  );

  mixerWindow.setEffects();

  const auto appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
  const scoped_actor self{actorSystem};
  self->anon_send(
    actor_cast<actor>(appStateManagerPtr),
    hydrate_display_a_v
  );

  Logging::write(
    Info,
    "Gui::MainWindow::setEffects",
    "Done setting effects."
  );
}

void MainWindow::connectActions() {
  const auto sceneLoadConnection = connect(&sceneLoadAction, &QAction::triggered, [&] {
    if (const int sceneDbId = sceneLoadAction.data().toULongLong(); gAppState->getSceneDbId() != sceneDbId) {
      Logging::write(
        Info,
        "Gui::MainWindow::sceneLoadAction",
        "Loading sceneId: " + std::to_string(sceneDbId)
      );

      mixer->loadScene(sceneDbId);
      setChannels();
      setEffects();

      Logging::write(
        Info,
        "Gui::MainWindow::sceneLoadAction",
        "Done loading sceneId: " + std::to_string(sceneDbId)
      );
    }
  });
}
} // Gui
} // Gj
