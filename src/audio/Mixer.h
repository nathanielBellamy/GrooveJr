//
// Created by ns on 1/12/25.
//

#ifndef GJAUDIOMIXER_H
#define GJAUDIOMIXER_H

#include "./Channel.h"
#include "./effects/EffectsChannel.h"

namespace Gj {
namespace Audio {

class Mixer {

  public:
    Channel mainChannel;
    Channel dryChannel;
    std::vector<Effects::EffectsChannel*> effectsChannels;

    Mixer();
    ~Mixer();

    bool addEffectsChannel();
    bool removeEffectsChannel(int idx);

};

} // Audio
} // Gj



#endif //GJAUDIOMIXER_H
