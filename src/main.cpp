//
// Created by ns on 9/30/24.
//

#include "main.h"

using namespace std::literals;
using namespace caf;
using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Gj {

Audio::Effects::Vst3::Host::App* PluginContext = new Audio::Effects::Vst3::Host::App();
std::vector<Audio::Effects::Vst3::Plugin*> vst3Plugins;

Audio::Effects::EffectMixer* EffectMixer = new Audio::Effects::EffectMixer();


void shutdown_handler(int sig) {
  std::cout << "Caught signal: " << sig << std::endl;
  std::cout << "Freeing resources..." << std::endl;

  for (auto &plugin : vst3Plugins) {
      plugin->audioHost->terminate();
      plugin->editorHost->terminate();
      delete plugin->audioHost;
      delete plugin->editorHost;
  }
  PluginContext->terminate();

  PluginContextFactory::instance().setPluginContext (nullptr);
  delete PluginContext;

  std::cout << "Done Freeing resources." << std::endl;
  std::cout << "== GrooveJr ==" << std::endl;
  exit(1);
}

void caf_main(int argc, char *argv[], actor_system& sys, std::vector<Audio::Effects::Vst3::Plugin*>& vst3Plugins) {

  // init Qt App
  auto qtApp = QApplication {argc, argv};
  auto mainWindow = Gj::Gui::MainWindow { sys };

  // init ActorSystem
  auto supervisor = sys.spawn(actor_from_state<Act::SupervisorState>, &mainWindow, vst3Plugins);

  mainWindow.show();
  qtApp.exec();
}

void initVst3PluginContext() {
    PluginContext = new Audio::Effects::Vst3::Host::App();
    PluginContextFactory::instance().setPluginContext (PluginContext);
}


extern "C" {
    int main(int argc, char *argv[]) {

        // setup signal handle
        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = shutdown_handler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, NULL);

        initVst3PluginContext();
        vst3Plugins.push_back(
            new Audio::Effects::Vst3::Plugin("/Library/Audio/Plug-Ins/VST3/TDR Nova.vst3" )
        );
        vst3Plugins.push_back(
            new Audio::Effects::Vst3::Plugin("/Library/Audio/Plug-Ins/VST3/ValhallaSupermassive.vst3" )
        );

        Audio::Effects::Vst3::Plugin::chainBuffers(vst3Plugins);

        // init actor system
        // Initialize the global type information before anything else.
        init_global_meta_objects<id_block::groovejr>();
        core::init_global_meta_objects();
        // Create the config.
        actor_system_config cfg;
        // Create the actor system.
        actor_system sys{cfg};
        // Run user-defined code.
        caf_main(argc, argv, sys, vst3Plugins);

        return 0;
    }
}

} // Gj