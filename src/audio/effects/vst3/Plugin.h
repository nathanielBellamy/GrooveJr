//
// Created by ns on 1/5/25.
//

#ifndef VST3PLUGIN_H
#define VST3PLUGIN_H

#include <vector>
#include <memory>

#include <sndfile.h>

#include "public.sdk/source/vst/utility/memoryibstream.h"

#include "../../../Logging.h"
#include "../../../AppState.h"
#include "../../../db/entity/mixer/Effect.h"
#include "../../../enums/Result.h"
#include "../../JackClient.h"
#include "./host/audiohost/source/audiohost.h"
#include "./host/editorhost/source/editorhost.h"
#include "../../../gui/Mixer/Channel/EffectsChannel/Effects/VstWindow.h"
#include "Util.h"


namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

using namespace Steinberg;

struct Plugin {
    AppState*                           gAppState;
    std::string                         name;
    const std::string                   path;
    VST3::Hosting::Module::Ptr          module;
    AudioHost::App*                     audioHost;
    EditorHost::App*                    editorHost;
    std::unique_ptr<ResizableMemoryIBStream> editorHostComponentStateStream;
    std::unique_ptr<ResizableMemoryIBStream> editorHostControllerStateStream;

    Plugin(
        std::string path,
        AppState* gAppState,
        std::shared_ptr<JackClient> jackClient
    );

    Plugin(
        const Db::Effect& effectEntity,
        AppState* gAppState,
        std::shared_ptr<JackClient> jackClient
    );

    ~Plugin();

    [[nodiscard]]
    FUnknownPtr<IAudioProcessor> getProcesser() const {
        return audioHost->audioClient->getComponent();
    }

    void initEditorHost(EditorHost::WindowPtr window);
    [[nodiscard]]
    Result cacheEditorHostState() const;
    [[nodiscard]]
    Result populateEditorHostStateBuffers(std::vector<uint8_t>& componentStateBuffer, std::vector<uint8_t>& controllerStateBuffer) const;
    [[nodiscard]]
    Result terminateEditorHost() const;

    [[nodiscard]]
    std::string getName() const { return name; };

    void setState(
        ResizableMemoryIBStream* audioHostComponentState,
        ResizableMemoryIBStream* audioHostControllerState,
        ResizableMemoryIBStream* editorHostComponentState,
        ResizableMemoryIBStream* editorHostControllerState
    ) const {
        audioHost->setState(audioHostComponentState, audioHostControllerState);
        if (editorHost != nullptr)
          editorHost->setState(editorHostComponentState, editorHostControllerState);
    }

    void loadState(
        const std::vector<uint8_t>& audioHostComponentStateBlob,
        const std::vector<uint8_t>& audioHostControllerStateBlob,
        const std::vector<uint8_t>& editorHostComponentStateBlob,
        const std::vector<uint8_t>& editorHostControllerStateBlob
    );

    void getState(
        ResizableMemoryIBStream* audioHostComponentState,
        ResizableMemoryIBStream* audioHostControllerState,
        ResizableMemoryIBStream* editorHostComponentState,
        ResizableMemoryIBStream* editorHostControllerState
    ) const {
        audioHost->getState(audioHostComponentState, audioHostControllerState);
        if (editorHost != nullptr)
          editorHost->getState(editorHostComponentState, editorHostControllerState);
    }
};

} // Vst3
} // Effects
} // Audio
} // Gj

#endif //VST3PLUGIN_H
