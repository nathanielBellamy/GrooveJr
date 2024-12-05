//
// Created by ns on 9/30/24.
//

#include "main.h"

using namespace caf;
using namespace std::literals;

namespace Gj {


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

    Steinberg::Vst::AudioHost::App* vst3AudioHost;
    vst3AudioHost = new Steinberg::Vst::AudioHost::App;
    const auto& vst3AudioHostCmdArgs = std::vector<std::string> {
        "/Library/Audio/Plug-Ins/VST3/ValhallaSupermassive.vst3"
    };
    vst3AudioHost->init(vst3AudioHostCmdArgs);

    Steinberg::Vst::HostApplication vst3HostApp;

    char *uuid1 = (char*) "0123456789ABCDEF";
    char *uuid2 = (char*) "0123456789GHIJKL";
    auto obj = (void*) malloc( 1000 * sizeof( Steinberg::Vst::HostMessage) );
    vst3HostApp.createInstance(uuid1, uuid2, &obj);

    Steinberg::Vst::EditorHost::App editorApp;
    const auto& cmdArgs = std::vector<std::string> {
        "/Library/Audio/Plug-Ins/VST3/ValhallaSupermassive.vst3"
    };
    editorApp.init (cmdArgs);
    return vst3AudioHost;
}

extern "C" {
    int main(int argc, char *argv[]) {
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