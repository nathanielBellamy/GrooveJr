#ifndef CASSETTE_H
#define CASSETTE_H

#include <string>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>

#include <sndfile.hh>
#include <portaudio.h>
#include "caf/actor_system.hpp"
#include "./audio_data.h"
#include "./caf_data.h"
#include "./Mixer.h"
#include "./ThreadStatics.h"
#include "./constants.h"

using namespace caf;

namespace Gj {
namespace Audio {

class Cassette
{
        actor_system& actorSystem;
        long threadId;
        char const* fileName;
        long initialFrameId;
        Mixer& mixer;

      public:
        Cassette(
            actor_system& actorSystem,
            long threadId,
            const char* fileName,
            long initialFrameId,
            Mixer& mixer
        );

        int play();

        void freeAudioData(AUDIO_DATA *audioData);

        static int callback(const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData );
};

} // Audio
} // Gj

#endif