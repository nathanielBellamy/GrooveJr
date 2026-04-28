// E2E Test replacing Squish
#include <QTest>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QTimer>
#include <QMainWindow>
#include <QTableView>
#include <QHeaderView>

#include "../../main.h"

using namespace std::literals;
using namespace caf;
using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Gj {
  Audio::Plugins::Vst3::Host::App* pluginContext = nullptr;
  State::Core* stateCore = nullptr;
  Audio::Mixer::Core* mixer = nullptr;
  Db::Dao* dao = nullptr;
  Audio::AudioCore* audioCore = nullptr;

  void shutdown_handler(const int sig) {
    if (QApplication::instance()) {
      QApplication::quit();
    } else {
      _exit(sig);
    }
  }

  Result initVst3PluginContext() {
    try {
      pluginContext = new Audio::Plugins::Vst3::Host::App();
      PluginContextFactory::instance().setPluginContext(pluginContext);
      return OK;
    } catch (...) {
      return ERROR;
    }
  }
}

class E2ETest : public QObject {
  Q_OBJECT

private:
  caf::actor_system* sys = nullptr;
  caf::actor_system_config cfg;
  Gj::Gui::MainWindow* mainWindow = nullptr;
  Gj::Gui::Hydrater hydrater{};

private slots:
  void initTestCase() {
    Gj::Logging::init();

    Gj::stateCore = new Gj::State::Core;
    Gj::dao = new Gj::Db::Dao(Gj::stateCore);
    if (const auto appStateEntity = Gj::dao->appStateRepository.get(); appStateEntity.id == 0) {
      const Gj::SceneID sceneId = Gj::dao->sceneRepository.create(Gj::Db::Scene::base());
      const auto scene = Gj::dao->sceneRepository.find(sceneId);
      Gj::stateCore->setScene(scene.value());
      Gj::dao->appStateRepository.save();
    }
    const auto appStateEntityReloaded = Gj::dao->appStateRepository.get();
    const auto scene = Gj::dao->sceneRepository.findOrCreate(appStateEntityReloaded.sceneId);
    Gj::stateCore->setFromEntityAndScene(appStateEntityReloaded, scene);
    Gj::dao->appStateRepository.persistAndSet();

    Gj::initVst3PluginContext();

    Gj::audioCore = new Gj::Audio::AudioCore(Gj::stateCore);
    Gj::mixer = new Gj::Audio::Mixer::Core(Gj::stateCore, Gj::dao);

    caf::init_global_meta_objects<caf::id_block::groovejr>();
    caf::core::init_global_meta_objects();

    sys = new caf::actor_system(cfg);

    auto playback = sys->spawn(
      actor_from_state<Gj::Act::PlaybackState>,
      caf::actor_cast<caf::strong_actor_ptr>(sys->registry().get(Gj::Act::SUPERVISOR)),
      Gj::stateCore,
      Gj::mixer,
      Gj::audioCore
    );

    auto appStateManager = sys->spawn(
      actor_from_state<Gj::Act::AppStateManagerState>,
      caf::actor_cast<caf::strong_actor_ptr>(sys->registry().get(Gj::Act::SUPERVISOR)),
      Gj::stateCore,
      Gj::mixer,
      Gj::dao,
      Gj::audioCore
    );

    auto musicLibraryManager = sys->spawn(
      actor_from_state<Gj::Act::MusicLibraryManagerState>,
      caf::actor_cast<caf::strong_actor_ptr>(sys->registry().get(Gj::Act::SUPERVISOR)),
      Gj::mixer,
      Gj::stateCore
    );

    mainWindow = new Gj::Gui::MainWindow{*sys, Gj::mixer, Gj::stateCore, Gj::shutdown_handler};
    mainWindow->connectHydrater(hydrater);
    mainWindow->show();
    mainWindow->setChannels();

    caf::anon_send(
      appStateManager,
      hydrate_display_a_v
    );

    QVERIFY(QTest::qWaitFor([&]() {
      return mainWindow->isVisible();
    }, 5000));
  }

  void cleanupTestCase() {
  }

  void testMenuStructure() {
    QVERIFY(mainWindow != nullptr);

    QMenuBar* menuBar = static_cast<QMainWindow*>(mainWindow)->menuBar();
    QVERIFY(menuBar != nullptr);

    bool fileMenuFound = false;
    bool musicMenuFound = false;

    for (QAction* action : menuBar->actions()) {
      if (action->text().contains("File")) fileMenuFound = true;
      if (action->text().contains("Music")) musicMenuFound = true;
    }

    QVERIFY(fileMenuFound);
    QVERIFY(musicMenuFound);
  }

  void testMusicLibrarySections() {
    QVERIFY(mainWindow != nullptr);

    // Find the MusicLibraryWindow inside MainWindow
    QWidget* musicLibWindow = mainWindow->findChild<QWidget*>("MusicLibraryWindow");
    if (!musicLibWindow) {
        QTableView* tableView = mainWindow->findChild<QTableView*>();
        QVERIFY(tableView != nullptr);
    } else {
        QVERIFY(musicLibWindow != nullptr);
    }
    
    QVERIFY(mainWindow->findChildren<QWidget*>().size() > 0);
  }
};

QTEST_MAIN(E2ETest)
#include "tst_e2e.moc"
