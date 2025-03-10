//
// Created by ns on 1/5/25.
//

#ifndef VST3PLUGIN_H
#define VST3PLUGIN_H

#include "./host/audiohost/source/audiohost.h"
#include "./host/editorhost/source/editorhost.h"

namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {

struct Plugin {
    int                                 audioFramesPerBuffer;
    const std::string&                  name;
    const std::string&                  path;
    Steinberg::Vst::AudioHost::App*     audioHost;
    Steinberg::Vst::EditorHost::App*    editorHost;

    Plugin(const std::string& path, int audioFramesPerBuffer);
    ~Plugin();

    void setAudioFramesPerBuffer(int framesPerBuffer) {
        audioFramesPerBuffer = framesPerBuffer;
        audioHost->setAudioFramesPerBuffer(framesPerBuffer);
    };

    void allocateBuffers() {
        audioHost->allocateBuffers();
    }

    Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor> getProcesser() {
        return audioHost->audioClient->getComponent();
    }
};


} // Vst3
} // Effects
} // Audio
} // Gj

#endif //VST3PLUGIN_H
