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
    std::vector<Effects::EffectsChannel*> effectsChannels;

  public:
    float* outputBuffer;

    Mixer();
    ~Mixer();

    bool addEffectsChannel();
    bool removeEffectsChannel(int idx);

    bool addEffectToChannel(int idx, Effects::Vst3::Plugin* effect);

    bool mixDown(int audioDataIndex, float* audioDataBuffer, int audioDataSfChannels, int framesPerBuffer);

    // TODO
    bool removeEffectFromChannel(int channelIdx, int effectIdx);

};

} // Audio
} // Gj



#endif //GJAUDIOMIXER_H
