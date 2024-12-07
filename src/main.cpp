//
// Created by ns on 9/30/24.
//

#include "main.h"

using namespace caf;
using namespace std::literals;

namespace Gj {

Steinberg::Vst::AudioHost::App* vst3AudioHost;
Steinberg::Vst::EditorHost::App* vst3EditorHost;

void shutdown_handler(int sig) {
  std::cout << "Caught signal: " << sig << std::endl;
  std::cout << "Freeing resources..." << std::endl;

  delete vst3AudioHost;
  delete vst3EditorHost;

  std::cout << "Done Freeing resources." << std::endl;
  std::cout << "== GrooveJr off ==" << std::endl;
  exit(1);
}

void caf_main(int argc, char *argv[], actor_system& sys, Steinberg::Vst::AudioHost::App* vst3AudioHost) {

  // init Qt App
  auto qtApp = QApplication {argc, argv};
  auto mainWindow = Gj::Gui::MainWindow { sys };

  // init ActorSystem
  auto supervisor = sys.spawn(actor_from_state<Act::SupervisorState>, &mainWindow, vst3AudioHost);

  mainWindow.show();
  qtApp.exec();
}

Steinberg::Vst::AudioHost::App* initVst3Host() {
    // alloc vst3AudioHostApp
//    Steinberg::Vst::IComponent* processorComponent;
//    Steinberg::Vst::IEditController* controller;
//    Steinberg::IPluginFactory* factory;
//    if ( factory->createInstance ("audioProcessorComponent", Steinberg::Vst::IComponent::iid, (void**)&processorComponent) ) {
//        std::cout << "Failed to create processorComponent" << std::endl;
//    } else {
//        std::cout << "Created processorComponent" << std::endl;
//    }

    const std::string& path = "/Library/Audio/Plug-Ins/VST3/ValhallaSupermassive.vst3";
    std::string error;
    VST3::Hosting::Module::Ptr module;
	module = VST3::Hosting::Module::create (path, error);
	if (!module)
	{
		std::string reason = "Could not create Module for file:";
		reason += path;
		reason += "\nError: ";
		reason += error;
//		Steinberg::IPlatform::instance ().kill (-1, reason);
	}
    Steinberg::Vst::AudioHost::App* vst3AudioHost = new Steinberg::Vst::AudioHost::App;
    vst3AudioHost->setModule(module);
    const auto& vst3AudioHostCmdArgs = std::vector<std::string> {
        "/Library/Audio/Plug-Ins/VST3/ValhallaSupermassive.vst3"
    };
    vst3AudioHost->init(vst3AudioHostCmdArgs);

//    Steinberg::Vst::HostApplication vst3HostApp;
//
//    char *uuid1 = (char*) "0123456789ABCDEF";
//    char *uuid2 = (char*) "0123456789GHIJKL";
//    auto obj = (void*) malloc( 1000 * sizeof( Steinberg::Vst::HostMessage) );
//    vst3HostApp.createInstance(uuid1, uuid2, &obj);

    vst3EditorHost = new Steinberg::Vst::EditorHost::App;
    vst3EditorHost->setModule(module);
    const auto& cmdArgs = std::vector<std::string> {
        "/Library/Audio/Plug-Ins/VST3/ValhallaSupermassive.vst3"
    };
    vst3EditorHost->init (cmdArgs);
    return vst3AudioHost;
}

extern "C" {
    int main(int argc, char *argv[]) {

        // setup signal handle
        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = shutdown_handler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, NULL);

        Steinberg::Vst::AudioHost::App* vst3AudioHost = initVst3Host();

        // init actor system
        // Initialize the global type information before anything else.
        init_global_meta_objects<id_block::groovejr>();
        core::init_global_meta_objects();
        // Create the config.
        actor_system_config cfg;
        // Create the actor system.
        actor_system sys{cfg};
        // Run user-defined code.
        caf_main(argc, argv, sys, vst3AudioHost);

        return 0;
    }
}

} // Gj