//
// Created by ns on 1/12/25.
//

#ifndef EFFECTCHANNEL_H
#define EFFECTCHANNEL_H

#include "./vst3/Plugin.h"
#include "./Channel.h"

namespace Gj {
namespace Audio {
namespace Effects {

class EffectChannel : public Channel {

  std::vector<Vst3::Plugin*> vst3Plugins;

  public:
    EffectChannel();
    ~EffectChannel();

    bool addPlugin();
    bool removePlugin();

    float getGain();
    void setGain(float gain);

    float getPan();
    void setPan(float pan);

    bool chainBuffers();
    bool unchainBuffers();

};

} // Effects
} // Audio
} // Gj



#endif //EFFECTCHANNEL_H
