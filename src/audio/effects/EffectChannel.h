//
// Created by ns on 1/12/25.
//

#ifndef EFFECTCHANNEL_H
#define EFFECTCHANNEL_H

#include "./vst3/Plugin.h"

namespace Gj {
namespace Audio {
namespace Effects {

class EffectChannel {

  std::vector<Vst3::Plugin*> vst3Plugins;
  float gain;
  float pan;

  public:

    EffectChannel();

    bool addPlugin();
    bool removePlugin();

    float getGain();
    void setGain(float gain);

    float getPan();
    void setPan(float pan);

};

} // Effects
} // Audio
} // Gj



#endif //EFFECTCHANNEL_H
