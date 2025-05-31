//
// Created by ns on 9/30/24.
//

#include "main.h"

using namespace std::literals;
using namespace caf;
using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Gj {

Audio::Effects::Vst3::Host::App* PluginContext;
AppState* gAppState;
Audio::Mixer* Mixer;
Db::Dao* Dao;

void shutdown_handler(const int sig) {
  Logging::write(
    Info,
    "shutdown_handler",
    "Caught Signal: " + std::to_string(sig)
  );

  delete Mixer;
  Logging::write(
    Info,
    "shutdown_handler",
    "Deleted Mixer"
  );

  delete gAppState;
  Logging::write(
    Info,
    "shutdown_handler",
    "Deleted gAppState"
  );

  PluginContext->terminate();
  PluginContextFactory::instance().setPluginContext (nullptr);
  delete PluginContext;
  Logging::write(
    Info,
    "shutdown_handler",
    "Deleted PluginContext"
  );

  delete Dao;
  Logging::write(
    Info,
    "shutdown_handler",
    "Deleted Dao"
  );

  Logging::write(
    Info,
    "shutdown_handler",
    "Done Freeing Resources"
  );
  Logging::write(
    Info,
    "shutdown_handler",
    "== GrooveJr =="
  );
  exit(sig);
}

void caf_main(int argc, char *argv[], void (*shutdown_handler) (int), actor_system& sys, AppState* gAppState, Audio::Mixer* mixer) {
  Logging::write(Info, "caf_main", "Starting caf_main");

  // init Qt App
  auto qtApp = QApplication {argc, argv};
  auto mainWindow = Gui::MainWindow { sys, mixer, shutdown_handler };

  Logging::write(Info, "caf_main", "Qt instantiated");

  // init ActorSystem
  auto supervisor = sys.spawn(
    actor_from_state<Act::SupervisorState>,
    gAppState,
    &mainWindow,
    mixer
  );

  Logging::write(Info, "caf_main", "Supervisor actor spawned");

  mainWindow.show();
  qtApp.exec();
}

void initVst3PluginContext() {
    PluginContext = new Audio::Effects::Vst3::Host::App();
    PluginContextFactory::instance().setPluginContext (PluginContext);
}

void initSql() {
}

extern "C" {
    int main(int argc, char *argv[]) {
        // setup signal handle
        struct sigaction sigIntHandler{};
        sigIntHandler.sa_handler = shutdown_handler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, nullptr);

        // setup logging
        Logging::init();
        Logging::write(
          Info,
          "main",
          "== GrooveJr =="
        );

        // setup Sql
        gAppState = new AppState;
        Dao = new Db::Dao(gAppState);
        gAppState->setFromEntity(Dao->appStateRepository.get());

        // setup Audio
        initVst3PluginContext();
        Mixer = new Audio::Mixer(gAppState);

        Logging::write(
          Info,
          "main",
          "Instantiated Mixer"
        );

        Mixer->addEffectsChannel(); // main
        Mixer->addEffectsChannel(); // channel 1
        // Mixer->addEffectsChannel();

        // init actor system
        // Initialize the global type information before anything else.
        init_global_meta_objects<id_block::groovejr>();
        core::init_global_meta_objects();
        // Create the config.
        actor_system_config cfg;
        // Create the actor system.
        actor_system sys{cfg};
        // Run user-defined code.
        caf_main(argc, argv, &shutdown_handler, sys, gAppState, Mixer);

        return 0;
    }
}

} // Gj