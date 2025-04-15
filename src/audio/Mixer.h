//
// Created by ns on 1/12/25.
//

#ifndef GJAUDIOMIXER_H
#define GJAUDIOMIXER_H

#include <iostream>
#include <cmath>
#include <algorithm>
#include <memory>

#include "../AppState.h"
#include "../Logging.h"
#include "./Channel.h"
#include "JackClient.h"
#include "./effects/EffectsChannel.h"
#include "../gui/Mixer/Channel/EffectsChannel/Effects/VstWindow.h"

namespace Gj {
namespace Audio {

class Mixer {
    AppState* gAppState;
    std::shared_ptr<JackClient> jackClient;
    Channel mainChannel;
    Channel dryChannel;
    float channelCount;
    std::vector<Effects::EffectsChannel*> effectsChannels;
    float** buffersA;
    float** buffersB;

    void incorporateLatencySamples(int latencySamples) const;

  public:
    float** inputBuffers;

    explicit Mixer(AppState*);
    ~Mixer();

    [[nodiscard]]
    jack_client_t* getJackClient() const {
      return jackClient.get()->getJackClient();
    };

    bool allocateBuffers();
    [[nodiscard]]
    bool freeBuffers() const;

    bool addEffectsChannel();
    bool removeEffectsChannel(int idx);
    [[nodiscard]]
    int effectsOnChannelCount(int idx) const;

    bool addEffectToChannel(int idx, const std::string& effectPath) const;
    void initEditorHostsOnChannel(int idx, std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const;
    void terminateEditorHostsOnChannel(int idx) const;

    bool setSampleRate(int sampleRate) const;

    bool mixDown(
      jack_default_audio_sample_t* outL,
      jack_default_audio_sample_t* outR,
      int audioDataIndex,
      const float* audioDataBuffer,
      int audioDataSfChannels,
      jack_nframes_t nframes) const;

    // TODO
    bool removeEffectFromChannel(int channelIdx, int effectIdx);

    int getAudioFramesPerBuffer() const { return gAppState->audioFramesPerBuffer; };
    void setAudioFramesPerBuffer(int framesPerBuffer) const { gAppState->audioFramesPerBuffer = framesPerBuffer; };

};

} // Audio
} // Gj



#endif //GJAUDIOMIXER_H
