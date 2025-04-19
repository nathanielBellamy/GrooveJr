#ifndef CASSETTE_H
#define CASSETTE_H

#include <string>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <variant>

#include <sndfile.hh>
#include <jack/jack.h>
#include <portaudio.h>

#include "public.sdk/samples/vst-hosting/audiohost/source/media/imediaserver.h"
#include "caf/actor_system.hpp"

#include "./AudioData.h"
#include "./Mixer.h"
#include "./ThreadStatics.h"
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
  Mixer* mixer;
  jack_client_t* jackClient;
  float* buffer{};
  SF_INFO sfInfo;
  SNDFILE* file{};
  AudioData audioData;

  static int jackProcessCallback(jack_nframes_t nframes, void* arg);
  AudioDataResult setupAudioData();
  int setupJack();

  public:
    Cassette(
        actor_system& actorSystem,
        long threadId,
        const char* fileName,
        long initialFrameId,
        Mixer* mixer
    );
    ~Cassette();

    int play();
    void cleanup() const;
};

} // Audio
} // Gj

#endif