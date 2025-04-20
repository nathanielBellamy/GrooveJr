#ifndef CASSETTE_H
#define CASSETTE_H

#include <string>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <array>
#include <vector>
#include <variant>

#include <sndfile.hh>
#include <jack/jack.h>

#include "public.sdk/samples/vst-hosting/audiohost/source/media/imediaserver.h"
#include "caf/actor_system.hpp"

#include "./AudioData.h"
#include "./Constants.h"
#include "./Mixer.h"
#include "./ThreadStatics.h"
#include "../AppState.h"
#include "Logging.h"

using namespace caf;

namespace Gj {
namespace Audio {

typedef std::variant<AudioData, int> AudioDataResult;

class Cassette
{
  actor_system& actorSystem;
  long threadId;
  char const* fileName;
  long initialFrameId;
  AppState* gAppState;
  Mixer* mixer;
  jack_client_t* jackClient;
  float* buffer{};
  SF_INFO sfInfo;
  SNDFILE* file{};
  float** inputBuffers; // full song audio data
  float** inputBuffersProcessHead; // where jack process callback should start
  float** buffersA;
  float** buffersB;
  float*** effectsChannelsWriteOutBuffer;
  AudioData audioData;

  static int jackProcessCallback(jack_nframes_t nframes, void* arg);
  AudioDataResult setupAudioData();
  IAudioClient::Buffers getPluginBuffers(const Effects::EffectsChannel* effectsChannel, int pluginIdx, const AudioData &audioData) const;
  int setupJack();

  bool allocateInputBuffers();
  bool populateInputBuffers() const;
  bool setupInputBuffers();

  bool allocateProcessBuffers();
  [[nodiscard]]
  bool freeBuffers() const;

  public:
    Cassette(
        actor_system& actorSystem,
        long threadId,
        const char* fileName,
        long initialFrameId,
        AppState* gAppState,
        Mixer* mixer
    );
    ~Cassette();

    int play();
    void cleanup() const;
};

} // Audio
} // Gj

#endif