#include "./Cassette.h"

#define PA_SAMPLE_TYPE      paFloat32

typedef float SAMPLE;

using namespace caf;

namespace Gj {
namespace Audio {

Cassette::Cassette(actor_system& actorSystem, long threadId, const char* fileName, long initialFrameId, Mixer* mixer)
  : actorSystem(actorSystem)
  , threadId(threadId)
  , fileName(fileName)
  , initialFrameId(initialFrameId)
  , mixer(mixer)
  {}

void Cassette::freeAudioData(AudioData *audioData) {
  free(audioData->buffer);
  sf_close(audioData->file);
  std::cout << "\nDone freeing resources for file: " << fileName;
};

// portaudio callback
// do not allocate/free memory within this method
// as it may be called at system-interrupt level
int Cassette::callback(const void *inputBuffer, void *outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void *userData )
{
  SAMPLE *out = (SAMPLE*) outputBuffer;
  unsigned int i;
  unsigned int c;
  (void) inputBuffer;
  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  AudioData *audioData = static_cast<AudioData*>(userData);

  // >> MIXER
  audioData->mixer->mixDown(audioData->index, audioData->buffer, audioData->sfinfo.channels, framesPerBuffer);

  for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
      *out++ = audioData->mixer->outputBuffer[i] * audioData->volume;
  }

  // TODO: early return here for testing
  audioData->index += framesPerBuffer * audioData->sfinfo.channels;
  return paContinue;

  // << MIXER

  if( audioData->buffer == NULL )
  {
//      for( i=0; i < framesPerBuffer; i++ )
//      {
//          *out++ = 0;  /* left - silent */
//          *out++ = 0;  /* right - silent */
//      }
     audioData->index = 0;
     audioData->readComplete = true;
     return paComplete;
  }
  else if (audioData->index > audioData->sfinfo.frames * audioData->sfinfo.channels - 1)
  {
     audioData->index = 0;
     audioData->readComplete = true;
     return paComplete;
  }
  else if (audioData->index < 0)
  {
    audioData->index = 0;
    audioData->readComplete = true;
    return paComplete;
  }
  // audioData->buffer --> paOut
  else if (audioData->playbackSpeed == -1.0) // reverse
  {
    for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
        *out++ = audioData->buffer[audioData->index - i] * audioData->volume;
    }

    audioData->index -= framesPerBuffer * audioData->sfinfo.channels;
    return paContinue;
  }
  else if (audioData->playbackSpeed == 0.5) // half-speed
  {
    int halfFramesPerBuffer = framesPerBuffer / 2; // framesPerBuffer is a power of 2
    for (i = 0; i < halfFramesPerBuffer * audioData->sfinfo.channels; i++) {
      *out++ = audioData->buffer[audioData->index + i] * audioData->volume;
      *out++ = audioData->buffer[audioData->index + i] * audioData->volume;
    }

    audioData->index += halfFramesPerBuffer * audioData->sfinfo.channels;
    return paContinue;
  }
  else if (audioData->playbackSpeed == -0.5) // half-speed reverse
  {
    int halfFramesPerBuffer = framesPerBuffer / 2; // framesPerBuffer is a power of 2
    for (i = 0; i < halfFramesPerBuffer * audioData->sfinfo.channels; i++) {
      *out++ = audioData->buffer[audioData->index - i] * audioData->volume;
      *out++ = audioData->buffer[audioData->index - i] * audioData->volume;
    }

    audioData->index -= halfFramesPerBuffer * audioData->sfinfo.channels;
    return paContinue;
  }
  else if (audioData->playbackSpeed == 2.0) // double speed
  {
    for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
      *out++ = audioData->buffer[audioData->index + (i * 2)] * audioData->volume;
    }

    audioData->index += (2 * framesPerBuffer) * audioData->sfinfo.channels;
    return paContinue;
  }
  else if (audioData->playbackSpeed == -2.0) // double speed reverse
  {
    for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
      *out++ = audioData->buffer[audioData->index - (2 * i)] * audioData->volume;
    }

    audioData->index -= (2 * framesPerBuffer) * audioData->sfinfo.channels;
    return paContinue;
  }
  else if (audioData->index < audioData->sfinfo.frames * audioData->sfinfo.channels)// play, regular speed
  {
    for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
        if (audioData->index + i < audioData->sfinfo.frames * audioData->sfinfo.channels) {
          *out++ = audioData->buffer[audioData->index + i] * audioData->volume;
        } else {
          audioData->index = 0;
          audioData->readComplete = true;
          return paComplete;
        }
    }
    audioData->index += framesPerBuffer * audioData->sfinfo.channels;

    return paContinue;
  } else {
    audioData->index = 0;
    audioData->readComplete = true;
    return paComplete;
  }
};

int Cassette::play()
{
  std::cout << "Playing Cassette..." << std::endl;

  // initialize data needed for audio playback
  sf_count_t index = 0;

  SF_INFO sfinfo;
  // https://svn.ict.usc.edu/svn_vh_public/trunk/lib/vhcl/libsndfile/doc/api.html
  // > When opening a file for read, the format field should be set to zero before calling sf_open().
  sfinfo.format = 0;
  SNDFILE *file;

  if (! (file = sf_open(fileName, SFM_READ, &sfinfo))) {
    std::cout << "Unable to open input file : sf : " << sf_strerror (NULL) << std::endl;
    return 1 ;
  };

  // Allocate memory for data
  float *buffer;
  buffer = (float *) malloc(sfinfo.frames * sfinfo.channels * sizeof(float));
  if (!buffer) {
      std::cout << "Cannot allocate memory for audioDataBuffer" << std::endl;
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(file, buffer, sfinfo.frames * sfinfo.channels);
  if (readcount == 0) {
      std::cout << "Cannot read file" << std::endl;
      return 1;
  }

  // update plugin effects with info about audio to be processed
  if (!mixer->setSampleRate(sfinfo.samplerate)) {
    std::cout << std::endl << "Unable to set Sample Rate " << sfinfo.samplerate << std::endl;
    goto error;
  }

  AudioData audioData(buffer, file, sfinfo, initialFrameId, readcount, Gj::PlayState::PLAY, mixer);

  // init cafData
  CAF_DATA cafData(actorSystem);

  // Init PA
  PaStreamParameters outputParameters; // inputParameters
  PaStream *stream;
  PaError err;

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  // // TODO: handle live input audio
  // inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
  // if (inputParameters.device == paNoDevice) {
  //     std::cout << "Error: No default input device." << std::endl;
  //     goto error;
  // }
  // inputParameters.channelCount = 1;
  // inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  // inputParameters.hostApiSpecificStreamInfo = NULL;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
      std::cout << "Error: No default output device." << std::endl;
      goto error;
  }
  outputParameters.channelCount = audioData.sfinfo.channels;
  outputParameters.sampleFormat = PA_SAMPLE_TYPE;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
            &stream,
            nullptr, // &inputParameters,
            &outputParameters,
            audioData.sfinfo.samplerate,
            mixer->getAudioFramesPerBuffer(),
            paNoFlag, /* paClipOn, */
            callback,
            &audioData );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

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

  err = Pa_StopStream( stream );
  if( err != paNoError ) goto error;

  err = Pa_CloseStream( stream );
  if( err != paNoError ) goto error;
  Pa_Terminate();
  freeAudioData(&audioData);
  return 0;

  error:
    Pa_Terminate();
    ThreadStatics::setPlayState(Gj::PlayState::STOP);
    freeAudioData(&audioData);
    std::cout << "An error occurred while using the portaudio stream" << std::endl;
    std::cout << "Error number: " << err << std::endl;
    std::cout << "Error message: " << Pa_GetErrorText( err ) << std::endl;
    return 1;
};

} // Audio
} // Gj
