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
Audio::AudioCore* audioCore;

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

void initVst3PluginContext() {
    PluginContext = new Audio::Effects::Vst3::Host::App();
    PluginContextFactory::instance().setPluginContext (PluginContext);
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
        const auto appStateEntity = Dao->appStateRepository.get();
        if (appStateEntity.id == 0) {
          // no appState in Db, init one
          if (Dao->appStateRepository.save() == 0)
            Logging::write(
              Error,
              "main",
              "Failed to save initial AppState"
            );
        }

        gAppState->setFromEntity(Dao->appStateRepository.get());
        Logging::write(
          Info,
          "main",
          "Loaded gAppState: id = " + std::to_string(gAppState->id) + " audioFramesPerBuffer = " + std::to_string(gAppState->getAudioFramesPerBuffer()) + ", sceneId: " + std::to_string(gAppState->getSceneId()) + ", sceneIndex: " + std::to_string(gAppState->getSceneIndex()) + "."
        );

        // setup Audio
        initVst3PluginContext();
        Logging::write(
          Info,
          "main",
          "Instantiated vst3PluginContext"
        );
        Mixer = new Audio::Mixer(gAppState, Dao);
        Logging::write(
          Info,
          "main",
          "Instantiated Mixer"
        );
        audioCore = new Audio::AudioCore(gAppState);
        Logging::write(
          Info,
          "main",
          "Instantiated audioCore"
        );

        // while (Mixer->getEffectsChannelsCount() < 1) // at least main and channel 1
        //   Mixer->addEffectsChannel();

        // init actor system
        // Initialize the global type information before anything else.
        init_global_meta_objects<id_block::groovejr>();
        core::init_global_meta_objects();
        // Create the config.
        actor_system_config cfg;
        // Create the actor system.
        actor_system sys{cfg};

        // spawn supervisor
        auto supervisor = sys.spawn(
        actor_from_state<Act::SupervisorState>,
          gAppState,
          Mixer,
          audioCore,
          shutdown_handler
        );

        return 0;
    }
}

} // Gj