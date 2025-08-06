//
// Created by ns on 4/6/25.
//

#ifndef GJAUDIOJACKCLIENT_H
#define GJAUDIOJACKCLIENT_H

#include "public.sdk/samples/vst-hosting/audiohost/source/media/imediaserver.h"

#include <cassert>

#include <jack/jack.h>
#include <jack/midiport.h>

#include "AudioData.h"
#include "Logging.h"
#include "../enums/Result.h"
#include "Constants.h"

namespace Gj {
namespace Audio {

// forward decl
class Mixer;

using namespace Steinberg::Vst;

//------------------------------------------------------------------------
//  jack Client
//------------------------------------------------------------------------
class JackClient final : public IMediaServer {
public:
  //--------------------------------------------------------------------
  using JackPorts = std::vector<jack_port_t *>;
  using JackName = std::string;

  explicit JackClient(Mixer* mixer);
  ~JackClient() override;

  Result initialize(JackName name);
  [[nodiscard]] Result activate(AudioData *audioData) const;
  [[nodiscard]] Result deactivate() const;

  [[nodiscard]]
  jack_client_t *getJackClient() const {
    return jackClient;
  }

  bool registerAudioClient(IAudioClient* client) override;
  bool registerMidiClient(IMidiClient *client) override;
  static int fillPlaybackBuffer(AudioData* audioData, sf_count_t playbackSpeed, jack_nframes_t nframes);

  //--------------------------------------------------------------------
private:
  Mixer* mixer;

  // Jack objects
  jack_client_t* jackClient = nullptr;
  JackPorts midiInputPorts;

  IAudioClient* audioClient = nullptr;
  IMidiClient* midiClient = nullptr;

  jack_client_t* registerClient(JackName name);
  Result setCallbacks(AudioData* audioData) const;

  static int processCallback(jack_nframes_t nframes, void* arg);
  static int setSampleRateCallback(jack_nframes_t nframes, void* arg);
  static int setBufferSizeCallback(jack_nframes_t nframes, void* arg);
  static int xRunCallback(void* arg);
  static Result logXRun();

  [[nodiscard]] Result setPorts() const;
  [[nodiscard]] Result activateAndConnectPorts() const;

  bool registerMidiPorts(IMidiClient *processor);
  bool addMidiInputPort(JackName name);
  int processMidi(jack_nframes_t nframes);
  bool autoConnectMidiPorts(jack_client_t *client);
  static float princArg(float phaseIn);
};

} // Audio
} // Gj

#endif //GJAUDIOJACKCLIENT_H
