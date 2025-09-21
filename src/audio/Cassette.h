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
#include <jack/ringbuffer.h>

#include "public.sdk/samples/vst-hosting/audiohost/source/media/imediaserver.h"
#include "caf/actor_system.hpp"

#include "./AudioCore.h"
#include "./Constants.h"
#include "./Mixer.h"
#include "./ThreadStatics.h"
#include "../AppState.h"
#include "JackClient.h"
#include "Logging.h"

using namespace caf;

namespace Gj {
namespace Audio {

class Cassette
{
  actor_system& actorSystem;
  long threadId;
  char const* fileName;
  long initialFrameId;
  AppState* gAppState;
  Mixer* mixer;
  float* buffer{};
  SF_INFO sfInfo;
  SNDFILE* file{};
  float* inputBuffer{};
  float* inputBuffers[2]{}; // full song audio data

  int setupAudioData();

  bool allocateInputBuffers();
  bool populateInputBuffers() const;
  bool setupInputBuffers();

  [[nodiscard]]
  bool deleteBuffers() const;

  public:
    Cassette(
        actor_system& actorSystem,
        AppState* gAppState,
        Mixer* mixer
    );
    ~Cassette();

    void cleanup();
};

} // Audio
} // Gj

#endif