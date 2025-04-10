#ifndef CASSETTE_H
#define CASSETTE_H

#include <string>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>

#include <sndfile.hh>
#include <jack/jack.h>
#include <portaudio.h>
#include "caf/actor_system.hpp"
#include "./AudioData.h"
#include "./Mixer.h"
#include "./ThreadStatics.h"
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
  Mixer* mixer;
  jack_client_t* jackClient;

  static int jackProcessCallback(jack_nframes_t nframes, void* arg);

  public:
    Cassette(
        actor_system& actorSystem,
        long threadId,
        const char* fileName,
        long initialFrameId,
        Mixer* mixer
    );

    int play();

    void freeAudioData(AudioData *audioData) const;

    // static int callback(const void *inputBuffer, void *outputBuffer,
    //                     unsigned long framesPerBuffer,
    //                     const PaStreamCallbackTimeInfo* timeInfo,
    //                     PaStreamCallbackFlags statusFlags,
    //                     void *userData );
};

} // Audio
} // Gj

#endif