//
// Created by ns on 1/5/25.
//

#ifndef VST3PLUGIN_H
#define VST3PLUGIN_H

#include <sndfile.h>

#include "../../../Logging.h"
#include "../../../AppState.h"
#include "../../JackClient.h"
#include "./host/audiohost/source/audiohost.h"
#include "./host/editorhost/source/editorhost.h"
#include "../../../gui/Mixer/Channel/EffectsChannel/Effects/VstWindow.h"


namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

struct Plugin {
    AppState*                           gAppState;
    std::string                         name;
    const std::string                   path;
    VST3::Hosting::Module::Ptr          module;
    AudioHost::App*                     audioHost;
    EditorHost::App*                    editorHost;

    Plugin(
        std::string path,
        AppState* gAppState,
        std::shared_ptr<JackClient> jackClient
    );
    ~Plugin();

    Steinberg::FUnknownPtr<IAudioProcessor> getProcesser() {
        return audioHost->audioClient->getComponent();
    }

    void initEditorHost(EditorHost::WindowPtr window);
    void terminateEditorHost() const;

    std::string getName() const { return name; };
};

} // Vst3
} // Effects
} // Audio
} // Gj

#endif //VST3PLUGIN_H
