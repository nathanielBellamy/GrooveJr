//
// Created by ns on 1/12/25.
//

#ifndef GJAUDIOMIXER_H
#define GJAUDIOMIXER_H

#include <iostream>
#include <cmath>
#include <algorithm>
#include "../AppState.h"
#include "./Channel.h"
#include "./effects/EffectsChannel.h"

namespace Gj {
namespace Audio {

class Mixer {
    AppState* gAppState;
    Channel mainChannel;
    Channel dryChannel;
    int channelCount;
    std::vector<Effects::EffectsChannel*> effectsChannels;

    void incorporateLatencySamples(int latencySamples) const;

  public:
    float** inputBuffers;
    float* outputBuffer;

    Mixer(AppState*);
    ~Mixer();

    bool allocateInputBuffers();
    bool freeInputBuffers() const;

    bool addEffectsChannel();
    bool removeEffectsChannel(int idx);

    bool addEffectToChannel(int idx, const std::string& effectPath) const;

    bool setSampleRate(int sampleRate) const;

    bool mixDown(int audioDataIndex, const float* audioDataBuffer, int audioDataSfChannels, int framesPerBuffer) const;

    // TODO
    bool removeEffectFromChannel(int channelIdx, int effectIdx);

    int getAudioFramesPerBuffer() const { return gAppState->audioFramesPerBuffer; };
    void setAudioFramesPerBuffer(int framesPerBuffer) const { gAppState->audioFramesPerBuffer = framesPerBuffer; };

};

} // Audio
} // Gj



#endif //GJAUDIOMIXER_H
