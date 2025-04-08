//
// Created by ns on 1/12/25.
//

#ifndef EFFECTSCHANNEL_H
#define EFFECTSCHANNEL_H

#include <memory>

#include "./vst3/Plugin.h"
#include "../Channel.h"
#include "../../AppState.h"
#include "../Logging.h"
#include "../JackClient.h"

namespace Gj {
namespace Audio {
namespace Effects {

class EffectsChannel {
  // NOTE:
  // fx0:inputBuffers->buffersA, fx1:buffersA->buffersB, fx2:buffersB->buffersA, fx3:buffersA->buffersB, ...
  AppState* gAppState;
  std::shared_ptr<JackClient> jackClient;
  int index;
  float** inputBuffers;
  float** buffersA;
  float** buffersB;
  std::vector<std::unique_ptr<Vst3::Plugin>> vst3Plugins;

  void allocateBuffers();
  void freeBuffers() const;
  [[nodiscard]] float** determineInputBuffers(int index) const;
  [[nodiscard]] float** determineOutputBuffers(int index) const;

  public:
    Channel channel;

    EffectsChannel(AppState* gAppState, std::shared_ptr<JackClient> jackClient, int index, float** inputBuffers);
    ~EffectsChannel();

    void process() const;

    [[nodiscard]] int getIndex() const { return index; }

    float getGain();
    void setGain(float gain);

    float getPan();
    void setPan(float pan);

    bool addEffect(const std::string& effectPath);
    [[nodiscard]] float** getBuffersWriteOut() const;
    void setSampleRate(int sampleRate) const;

    int effectCount() const;
    void initEditorHosts(const std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const;
    void terminateEditorHosts() const;

};

} // Effects
} // Audio
} // Gj



#endif //EFFECTCHANNEL_H
