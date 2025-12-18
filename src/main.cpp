//
// Created by ns on 9/30/24.
//

#include "main.h"

using namespace std::literals;
using namespace caf;
using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Gj {
Audio::Plugins::Vst3::Host::App* pluginContext;
AppState* gAppState;
Audio::Mixer::Core* mixer;
Db::Dao* dao;
Audio::AudioCore* audioCore;

void shutdown_handler(const int sig) {
  Logging::write(
    Info,
    "shutdown_handler",
    "Caught Signal: " + std::to_string(sig)
  );

  delete mixer;
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

  delete dao;
  Logging::write(
    Info,
    "shutdown_handler",
    "Deleted Dao"
  );

  delete audioCore;
  Logging::write(
    Info,
    "shutdown_handler",
    "Deleted AudioCore"
  );

  Logging::write(
    Info,
    "shutdown_handler",
    "Done Freeing Resources"
  );

  pluginContext->terminate();
  PluginContextFactory::instance().setPluginContext(nullptr);
  delete pluginContext;
  Logging::write(
    Info,
    "shutdown_handler",
    "Deleted PluginContext"
  );

  Logging::write(
    Info,
    "shutdown_handler",
    "== GrooveJr =="
  );
  exit(sig);
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

extern "C" {
int main(int argc, char* argv[]) {
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
  dao = new Db::Dao(gAppState);
  if (const auto appStateEntity = dao->appStateRepository.get(); appStateEntity.id == 0) {
    // no appState in Db, init one
    const SceneID sceneId = dao->sceneRepository.create(Db::Scene::base());
    const auto scene = dao->sceneRepository.find(sceneId);
    if (!scene) {
      Logging::write(
        Error,
        "main",
        "Unable to init Scene"
      );
      throw std::runtime_error("Unable to init Scene");
    }
    gAppState->setScene(scene.value());
    if (dao->appStateRepository.save() == 0) {
      Logging::write(
        Error,
        "main",
        "Failed to save initial AppState"
      );
      throw std::runtime_error("Failed to save initial AppState");
    }
  }

  const auto appStateEntityReloaded = dao->appStateRepository.get();
  const auto scene = dao->sceneRepository.findOrCreate(appStateEntityReloaded.sceneId);
  gAppState->setFromEntityAndScene(
    appStateEntityReloaded,
    scene
  );
  dao->appStateRepository.persistAndSet();

  Logging::write(
    Info,
    "main",
    "Loaded gAppState: " + gAppState->toString()
  );

  if (initVst3PluginContext() == OK) {
    Logging::write(
      Info,
      "main",
      "Instantiated VST3 PluginContext"
    );
  } else {
    Logging::write(
      Error,
      "main::initVst3PluginContext",
      "Failed to initialize Vst3PluginContext"
    );
    return 1;
  }

  audioCore = new Audio::AudioCore(gAppState);
  Logging::write(
    Info,
    "main",
    "Instantiated audioCore"
  );

  mixer = new Audio::Mixer::Core(gAppState, dao);
  Logging::write(
    Info,
    "main",
    "Instantiated Mixer"
  );

  // init actor system
  init_global_meta_objects<id_block::groovejr>();
  core::init_global_meta_objects();
  actor_system_config cfg;
  actor_system sys{cfg};

  auto supervisor = sys.spawn(
    actor_from_state<Act::SupervisorState>,
    gAppState,
    mixer,
    audioCore,
    shutdown_handler
  );
  return 0;
}
} // extern C
} // Gj
