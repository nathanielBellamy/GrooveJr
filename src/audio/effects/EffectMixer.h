//
// Created by ns on 1/12/25.
//

#ifndef EFFECTMIXER_H
#define EFFECTMIXER_H

#include "./Channel.h"
#include "./EffectChannel.h"

namespace Gj {
namespace Audio {
namespace Effects {


class EffectMixer {

  public:
    Channel* dryChannel;
    std::vector<EffectChannel*> wetChannels;

    EffectMixer();
    ~EffectMixer();

    bool addChannel();
    bool removeChannel();

};

} // Effects
} // Audio
} // Gj



#endif //EFFECTMIXER_H
