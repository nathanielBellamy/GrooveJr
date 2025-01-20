//
// Created by ns on 1/12/25.
//

#ifndef GJAUDIOMIXER_H
#define GJAUDIOMIXER_H

#include "./Channel.h"
#include "./constants.h"
#include "./effects/EffectsChannel.h"

namespace Gj {
namespace Audio {

class Mixer {
    Channel mainChannel;
    Channel dryChannel;
    int channelCount;
    std::vector<Effects::EffectsChannel*> effectsChannels;

  public:
    float** inputBuffers;
    float* outputBuffer;

    Mixer();
    ~Mixer();

    bool addEffectsChannel();
    bool removeEffectsChannel(int idx);

    bool addEffectToChannel(int idx, Effects::Vst3::Plugin* effect) const;

    bool mixDown(int audioDataIndex, const float* audioDataBuffer, int audioDataSfChannels, int framesPerBuffer) const;

    // TODO
    bool removeEffectFromChannel(int channelIdx, int effectIdx);

};

} // Audio
} // Gj



#endif //GJAUDIOMIXER_H
