#include "./Cassette.h"

#define PA_SAMPLE_TYPE      paFloat32

typedef float SAMPLE;

using namespace caf;

namespace Gj {
namespace Audio {

jack_port_t* outPortL;
jack_port_t* outPortR;

Cassette::Cassette(actor_system& actorSystem, long threadId, const char* fileName, long initialFrameId, Mixer* mixer)
  : actorSystem(actorSystem)
  , threadId(threadId)
  , fileName(fileName)
  , initialFrameId(initialFrameId)
  , mixer(mixer)
  , jackClient(mixer->getJackClient())
  {}

void Cassette::freeAudioData(AudioData *audioData) const {
  free(audioData->buffer);
  sf_close(audioData->file);
  Logging::write(
    Info,
    "Cassette::freeAudioData",
    "Done freeing resources for file" + std::string(fileName)
  );
};

// jack process callback
// do not allocate/free memory within this method
// as it may be called at system-interrupt level
int Cassette::jackProcessCallback(jack_nframes_t nframes, void* arg) {
  auto* outL = static_cast<jack_default_audio_sample_t*>(
    jack_port_get_buffer(outPortL, nframes)
  );
  auto* outR = static_cast<jack_default_audio_sample_t*>(
    jack_port_get_buffer(outPortR, nframes)
  );

  auto *audioData = static_cast<AudioData*>(arg);
  const int channelCount = audioData->sfinfo.channels;
  audioData->mixer->mixDown(outL, outR, audioData->index, audioData->buffer, channelCount, nframes);

  audioData->index += nframes * channelCount;
  return 0;
}

// TODO:
  // - cleanup once JACK is working
// portaudio callback
// do not allocate/free memory within this method
// as it may be called at system-interrupt level
// int Cassette::callback(const void *inputBuffer, void *outputBuffer,
//                     unsigned long framesPerBuffer,
//                     const PaStreamCallbackTimeInfo* timeInfo,
//                     PaStreamCallbackFlags statusFlags,
//                     void *userData )
// {
//   SAMPLE *out = (SAMPLE*) outputBuffer;
//   unsigned int i;
//   unsigned int c;
//   (void) inputBuffer;
//   (void) timeInfo; /* Prevent unused variable warnings. */
//   (void) statusFlags;
//   AudioData *audioData = static_cast<AudioData*>(userData);
//
//   // >> MIXER
//   audioData->mixer->mixDown(audioData->index, audioData->buffer, audioData->sfinfo.channels, framesPerBuffer);
//
//   for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
//       *out++ = audioData->mixer->outputBuffer[i] * audioData->volume;
//   }
//
//   // TODO: early return here for testing
//   audioData->index += framesPerBuffer * audioData->sfinfo.channels;
//   return paContinue;
//
//   // << MIXER
//
//   if( audioData->buffer == NULL )
//   {
// //      for( i=0; i < framesPerBuffer; i++ )
// //      {
// //          *out++ = 0;  /* left - silent */
// //          *out++ = 0;  /* right - silent */
// //      }
//      audioData->index = 0;
//      audioData->readComplete = true;
//      return paComplete;
//   }
//   else if (audioData->index > audioData->sfinfo.frames * audioData->sfinfo.channels - 1)
//   {
//      audioData->index = 0;
//      audioData->readComplete = true;
//      return paComplete;
//   }
//   else if (audioData->index < 0)
//   {
//     audioData->index = 0;
//     audioData->readComplete = true;
//     return paComplete;
//   }
//   // audioData->buffer --> paOut
//   else if (audioData->playbackSpeed == -1.0) // reverse
//   {
//     for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
//         *out++ = audioData->buffer[audioData->index - i] * audioData->volume;
//     }
//
//     audioData->index -= framesPerBuffer * audioData->sfinfo.channels;
//     return paContinue;
//   }
//   else if (audioData->playbackSpeed == 0.5) // half-speed
//   {
//     int halfFramesPerBuffer = framesPerBuffer / 2; // framesPerBuffer is a power of 2
//     for (i = 0; i < halfFramesPerBuffer * audioData->sfinfo.channels; i++) {
//       *out++ = audioData->buffer[audioData->index + i] * audioData->volume;
//       *out++ = audioData->buffer[audioData->index + i] * audioData->volume;
//     }
//
//     audioData->index += halfFramesPerBuffer * audioData->sfinfo.channels;
//     return paContinue;
//   }
//   else if (audioData->playbackSpeed == -0.5) // half-speed reverse
//   {
//     int halfFramesPerBuffer = framesPerBuffer / 2; // framesPerBuffer is a power of 2
//     for (i = 0; i < halfFramesPerBuffer * audioData->sfinfo.channels; i++) {
//       *out++ = audioData->buffer[audioData->index - i] * audioData->volume;
//       *out++ = audioData->buffer[audioData->index - i] * audioData->volume;
//     }
//
//     audioData->index -= halfFramesPerBuffer * audioData->sfinfo.channels;
//     return paContinue;
//   }
//   else if (audioData->playbackSpeed == 2.0) // double speed
//   {
//     for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
//       *out++ = audioData->buffer[audioData->index + (i * 2)] * audioData->volume;
//     }
//
//     audioData->index += (2 * framesPerBuffer) * audioData->sfinfo.channels;
//     return paContinue;
//   }
//   else if (audioData->playbackSpeed == -2.0) // double speed reverse
//   {
//     for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
//       *out++ = audioData->buffer[audioData->index - (2 * i)] * audioData->volume;
//     }
//
//     audioData->index -= (2 * framesPerBuffer) * audioData->sfinfo.channels;
//     return paContinue;
//   }
//   else if (audioData->index < audioData->sfinfo.frames * audioData->sfinfo.channels)// play, regular speed
//   {
//     for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
//         if (audioData->index + i < audioData->sfinfo.frames * audioData->sfinfo.channels) {
//           *out++ = audioData->buffer[audioData->index + i] * audioData->volume;
//         } else {
//           audioData->index = 0;
//           audioData->readComplete = true;
//           return paComplete;
//         }
//     }
//     audioData->index += framesPerBuffer * audioData->sfinfo.channels;
//
//     return paContinue;
//   } else {
//     audioData->index = 0;
//     audioData->readComplete = true;
//     return paComplete;
//   }
// };

int Cassette::setupJack(AudioData& audioData) const {
  int setProcessResult = jack_set_process_callback(
    jackClient,
    &Cassette::jackProcessCallback,
    &audioData
  );
  if ( setProcessResult == 0) {
    Logging::write(
      Info,
      "Cassette::setupJack",
      "Set Jack process callback"
    );
  } else {
    Logging::write(
      Error,
      "Cassette::setupJack",
      "Unable to set process callback - status: " + std::to_string(setProcessStatus)
    );
    return 1;
  }

  if (outPortL == nullptr) {
      outPortL = jack_port_register(
        jackClient,
        "out_port_L",
        JACK_DEFAULT_AUDIO_TYPE,
        JackPortIsOutput,
        0
      );
  }

  if (outPortL == nullptr) {
    Logging::write(
      Error,
      "Cassette::play",
      "Unable to create Jack outPortL"
    );
    return 2;
  }

  if (outPortR == nullptr) {
    outPortR = jack_port_register(
      jackClient,
      "out_port_R",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput,
      0
    );
  }
  if (outPortR == nullptr) {
    Logging::write(
      Error,
      "Cassette::play",
      "Unable to create Jack outPortR"
    );
    return 3;
  }

  int jackActivateStatus = jack_activate(jackClient);
  if (jackActivateStatus == 0) {
    Logging::write(
      Info,
      "Cassette::play",
      "Jack activated successfully"
    );
  } else {
    Logging::write(
      Error,
      "Cassette::play",
      "Unable to activate jack - status: " + std::to_string(jackActivateStatus)
    );
    return 4;
  }

  int connectStatusL = jack_connect(jackClient, jack_port_name(outPortL), "system:playback_1");
  if (connectStatusL != 0) {
    Logging::write(
      Error,
      "Cassette::play()",
      "Unable to connect out_port_L - status: " + std::to_string(connectStatusL)
    );
    return 5;
  }

  int connectStatusR = jack_connect(jackClient, jack_port_name(outPortR), "system:playback_2");
  if (connectStatusR != 0) {
      Logging::write(
        Error,
        "Cassette::play()",
        "Unable to connect out_port_R - status: " + std::to_string(connectStatusR)
      );
      return 6;
  }

  // TESTING SHOW PORTS
  const char** ports = jack_get_ports(jackClient, nullptr, nullptr, 0);
  if (ports == nullptr) {
    printf("No ports found.\n");
  } else {
    for (int i = 0; ports[i] != nullptr; i++) {
      printf("Port: %s\n", ports[i]);
    }
  }
  // TESTING SHOW PORTS

  return 0;
}

int Cassette::play() const {
  Logging::write(
    Info,
    "Cassette::play()",
    "Playing Cassette..."
  );

  if (jackClient == nullptr) {
    Logging::write(
      Error,
      "Cassette::play()",
      "jackClient is null"
    );
  }

  // initialize data needed for audio playback
  sf_count_t index = 0;

  SF_INFO sfinfo;
  // https://svn.ict.usc.edu/svn_vh_public/trunk/lib/vhcl/libsndfile/doc/api.html
  // > When opening a file for read, the format field should be set to zero before calling sf_open().
  sfinfo.format = 0;
  SNDFILE *file;

  if (! (file = sf_open(fileName, SFM_READ, &sfinfo))) {
    Logging::write(
      Info,
      "Cassette::play()",
      "Unable to open input file : sf : " + std::string(sf_strerror(NULL))
    );
    return 1 ;
  };

  // Allocate memory for data
  float *buffer;
  buffer = (float *) malloc(sfinfo.frames * sfinfo.channels * sizeof(float));
  if (!buffer) {
      Logging::write(
        Info,
        "Cassette::play()",
        "Cannot allocate memory for audioDataBuffer"
      );
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(file, buffer, sfinfo.frames * sfinfo.channels);
  if (readcount == 0) {
    Logging::write(
      Error,
      "Cassette::play",
      "Unable to read file: " + std::string(fileName)
    );
      return 1;
  }

  // setup jack
  AudioData audioData(buffer, file, sfinfo, initialFrameId, readcount, Gj::PlayState::PLAY, mixer);

  // update plugin effects with info about audio to be processed
  if (!mixer->setSampleRate(sfinfo.samplerate)) {
    Logging::write(
      Error,
      "Cassette::play",
      "Unable to set sample rate: " + std::to_string(sfinfo.samplerate)
    );
    goto error;
  }

  int setupJackStatus = setupJack(audioData);
  if (setupJackStatus == 0) {
    Logging::write(
      Info,
      "Cassette::play()",
      "Setup Jack"
    );
  } else {
    Logging::write(
      Error,
      "Cassette::play()",
      "Unable to setup Jack - status: " + std::to_string(setupJackStatus)
    );
    goto error;
  }


  while(
          audioData.playState != Gj::PlayState::STOP
            && audioData.playState != Gj::PlayState::PAUSE
            && audioData.index > -1
            && audioData.index < audioData.sfinfo.frames * audioData.sfinfo.channels - 1
  ) // 0: STOP, 1: PLAY, 2: PAUSE, 3: RW, 4: FF
  {
    // hold thread open until stopped

    // here is our chance to pull data out of the application
    // and
    // make it accessible to our running audio callback through the audioData obj

    if (audioData.readComplete) { // reached end of input file
        ThreadStatics::setPlayState(Gj::PlayState::STOP);
        ThreadStatics::setReadComplete(true);
        Logging::write(
          Info,
          "Cassette::play",
          "Read complete"
        );
        break;
    }

    if (audioData.fadeIn > 0.01) {
        audioData.fadeIn -= 0.01;
        audioData.volume += 0.01;
    }

    if ( threadId != Gj::Audio::ThreadStatics::getThreadId() ) { // fadeout, break + cleanup
        if (audioData.fadeOut < 0.01) { // break + cleanup
            break;
        } else { // continue fading out
            audioData.volume -= 0.001;
            audioData.fadeOut -= 0.001;
        }
    } else {
        audioData.playbackSpeed = ThreadStatics::getPlaybackSpeed();
        audioData.playState = ThreadStatics::getPlayState();
        ThreadStatics::setFrameId( (long) audioData.index );
    }

    std::this_thread::sleep_for( std::chrono::milliseconds(10) );
  } // end of while loop

  if ( threadId == ThreadStatics::getThreadId() ) { // current audio thread has reached natural end of file
      if (audioData.playState == Gj::PlayState::PLAY) {
          ThreadStatics::setPlayState(Gj::PlayState::STOP);
      } else {
          ThreadStatics::setPlayState(audioData.playState);
      }
      ThreadStatics::setReadComplete(true);
  }

  jack_deactivate(jackClient);
  // err = Pa_StopStream( stream );
  // if( err != paNoError ) goto error;
  //
  // err = Pa_CloseStream( stream );
  // if( err != paNoError ) goto error;
  // Pa_Terminate();
  freeAudioData(&audioData);
  return 0;

  error:
    Pa_Terminate();
    ThreadStatics::setPlayState(Gj::PlayState::STOP);
    freeAudioData(&audioData);
    Logging::write(
      Error,
      "Cassette::play",
      "An Error occurred during audio playback"
    );
    return 1;
};

} // Audio
} // Gj
