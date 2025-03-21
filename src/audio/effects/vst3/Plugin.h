//
// Created by ns on 1/5/25.
//

#ifndef VST3PLUGIN_H
#define VST3PLUGIN_H

#include "../../../Logging.h"
#include "../../../AppState.h"
#include "./host/audiohost/source/audiohost.h"
#include "./host/editorhost/source/editorhost.h"
#include "../../../gui/Mixer/Effects/VstWindow.h"


namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

struct Plugin {
    AppState*                           gAppState;
    const std::string&                  name;
    const std::string&                  path;
    Steinberg::Vst::AudioHost::App*     audioHost;
    Steinberg::Vst::EditorHost::App*    editorHost;

    Plugin(
        const std::string& path,
        AppState* gAppState,
        float** inputBuffers,
        float** outputBuffers,
        Steinberg::Vst::EditorHost::WindowPtr window
    );
    ~Plugin();

    Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor> getProcesser() {
        return audioHost->audioClient->getComponent();
    }
};


} // Vst3
} // Effects
} // Audio
} // Gj

#endif //VST3PLUGIN_H
