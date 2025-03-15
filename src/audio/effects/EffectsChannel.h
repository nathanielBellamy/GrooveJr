//
// Created by ns on 1/12/25.
//

#ifndef EFFECTSCHANNEL_H
#define EFFECTSCHANNEL_H

#include "./vst3/Plugin.h"
#include "../Channel.h"
#include "../Logging.h"

namespace Gj {
namespace Audio {
namespace Effects {

class EffectsChannel {
  // NOTE:
  // fx0:inputBuffers->buffersA, fx1:buffersA->buffersB, fx2:buffersB->buffersA, fx3:buffersA->buffersB, ...
  int audioFramesPerBuffer;
  float** inputBuffers;
  float** buffersA;
  float** buffersB;
  void allocateBuffers();

  public:
    Channel channel;
    std::vector<Vst3::Plugin*> vst3Plugins;

    EffectsChannel(float** inputBuffers, int audioFramesPerBuffer);
    ~EffectsChannel();

    bool addPlugin();
    bool removePlugin();

    float getGain();
    void setGain(float gain);

    float getPan();
    void setPan(float pan);

    bool chainBuffers() const;
    bool unchainBuffers() const;

    bool addEffect(Vst3::Plugin *plugin);


};

} // Effects
} // Audio
} // Gj



#endif //EFFECTCHANNEL_H
