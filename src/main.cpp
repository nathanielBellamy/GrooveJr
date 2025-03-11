//
// Created by ns on 9/30/24.
//

#include "main.h"

using namespace std::literals;
using namespace caf;
using namespace Steinberg;
using namespace Steinberg::Vst;

enum severity_level {
  info,
  notification,
  warning,
  error,
  critical
};
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(gjLog, boost::log::sources::severity_logger<severity_level>);

namespace Gj {

auto PluginContext = new Audio::Effects::Vst3::Host::App();
auto gAppState = new AppState(128, PlayState::STOP);
auto Mixer = new Audio::Mixer(gAppState);

void shutdown_handler(const int sig) {
  std::cout << "Caught signal: " << sig << std::endl;
  std::cout << "Freeing resources..." << std::endl;

  delete Mixer;
  delete gAppState;

  PluginContext->terminate();
  PluginContextFactory::instance().setPluginContext (nullptr);
  delete PluginContext;

  std::cout << "Done Freeing resources." << std::endl;
  std::cout << "== GrooveJr ==" << std::endl;
  exit(sig);
}

void initLogging() {
  boost::log::register_simple_formatter_factory< severity_level, char >("Severity");
  boost::log::add_file_log(
      boost::log::keywords::file_name = "/Users/ns/code/GrooveJr/logs/groove_jr_%N.log",
      boost::log::keywords::rotation_size = 10 * 1024 * 1024,
      boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = "[%TimeStamp%] :: %Severity% :: %Caller% :: %Message%"
  );
  boost::log::add_common_attributes();

  boost::log::sources::severity_logger< severity_level > slg;

  boost::log::sources::severity_logger<severity_level> lg = gjLog::get();
  lg.add_attribute("Caller", boost::log::attributes::constant<std::string>("initLogging"));
  BOOST_LOG_SEV(lg, info) << "GrooveJr log initialized";

  // boost::log::core::get()->set_filter(
  //     boost::log::trivial::severity >= boost::log::trivial::info
  // );
}

void caf_main(int argc, char *argv[], void (*shutdown_handler) (int), actor_system& sys, AppState* gAppState, Audio::Mixer* mixer) {
  boost::log::sources::severity_logger<severity_level> lg = gjLog::get();
  lg.add_attribute("Caller", boost::log::attributes::constant<std::string>("caf_main"));

  // init Qt App
  auto qtApp = QApplication {argc, argv};
  auto mainWindow = Gui::MainWindow { sys, shutdown_handler };

  BOOST_LOG_SEV(lg, info) << "qtApp instantiated";

  // init ActorSystem
  auto supervisor = sys.spawn(
    actor_from_state<Act::SupervisorState>,
    gAppState,
    &mainWindow,
    mixer
  );

  BOOST_LOG_SEV(lg, info) << "Supervisor actor spawned";

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
        struct sigaction sigIntHandler{};
        sigIntHandler.sa_handler = shutdown_handler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, nullptr);

        // setup logging
        initLogging();

        initVst3PluginContext();

        Mixer->addEffectsChannel();
        Mixer->addEffectsChannel();
        Mixer->addEffectsChannel();
        // Mixer->addEffectToChannel(
        //     0,
        //     "/Library/Audio/Plug-Ins/VST3/TDR Nova.vst3"
        // );
        // Mixer->addEffectToChannel(
        //     0,
        //     "/Library/Audio/Plug-Ins/VST3/ValhallaSupermassive.vst3"
        // );


        // Mixer->addEffectToChannel(
        //    1,
        //    new Audio::Effects::Vst3::Plugin("/Library/Audio/Plug-Ins/VST3/TDR Nova.vst3" )
        // );
        //Mixer->addEffectToChannel(
        //    1,
        //    new Audio::Effects::Vst3::Plugin("/Library/Audio/Plug-Ins/VST3/ValhallaSupermassive.vst3" )
        //);

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