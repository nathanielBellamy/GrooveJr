#include "./Cassette.h"

#define PA_SAMPLE_TYPE      paFloat32

typedef float SAMPLE;

using namespace caf;

namespace Gj {
namespace Audio {

Cassette::Cassette(actor_system& actorSystem, long threadId, const char* fileName, long initialFrameId, Steinberg::Vst::AudioHost::App* vst3Host) :
  actorSystem(actorSystem)
  , threadId(threadId)
  , fileName(fileName)
  , initialFrameId(initialFrameId)
  , vst3Host(vst3Host)
  {}

void Cassette::freeAudioData(AUDIO_DATA *audioData) {
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
  AUDIO_DATA *audioData = (AUDIO_DATA*) userData;

  // >> VST PROCESSING
//  Steinberg::Vst::AudioHost::App* vst3Host;
//  vst3Host = reinterpret_cast<Steinberg::Vst::AudioHost::App*>(audioData->vst3AudioHost);
//
//  // populate input buffers
//  for (c = 0; c < audioData->sfinfo.channels; c++) {
//      for (i = 0; i < framesPerBuffer; i++) {
//        vst3Host->chowTapeModelBuffers.inputs[c][i] = audioData->buffer[audioData->index + i + (framesPerBuffer * c)] * audioData->volume;
//      }
//  }
//
//  // process
//  vst3Host->vst3Processor->process(vst3Host->chowTapeModelBuffers, (int64_t) framesPerBuffer);
//
//  // write output buffers to output
//  for (i = 0; i < framesPerBuffer ; i++) {
//      for (c = 0; c < audioData->sfinfo.channels; c++) {
//          *out++ = vst3Host->chowTapeModelBuffers.outputs[c][i] * audioData->volume;
//      }
//  }
//
//  // TODO: early return here for testing
//  audioData->index += framesPerBuffer * audioData->sfinfo.channels;
//  return paContinue;
  // << VST PROCESSING

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

int Cassette::run()
{
  // intialize data needed for audio playback
  sf_count_t index = 0;

  SF_INFO sfinfo;
  // https://svn.ict.usc.edu/svn_vh_public/trunk/lib/vhcl/libsndfile/doc/api.html
  // > When opening a file for read, the format field should be set to zero before calling sf_open().
  sfinfo.format = 0;
  SNDFILE *file;

//  if (! (file = sf_open("gs_music_library/Unknown Artist/Unknown Album/test.mp3", SFM_READ, &sfinfo)))
  if (! (file = sf_open(fileName, SFM_READ, &sfinfo)))
  {
    printf ("Not able to open input file.\n") ;
    /* Print the error message from libsndfile. */
    puts (sf_strerror (NULL)) ;
    return 1 ;
  };

  // Allocate memory for data
  float *buffer;
  buffer = (float *) malloc(sfinfo.frames * sfinfo.channels * sizeof(float));
  if (!buffer) {
      printf("\nCannot allocate memory");
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(file, buffer, sfinfo.frames * sfinfo.channels);
  if (readcount == 0) {
      printf("\nCannot read file");
      return 1;
  }

//  sf_count_t initialFrameId = (sf_count_t) initialFrameId;
  AUDIO_DATA audioData(buffer, file, sfinfo, initialFrameId, readcount, 1, vst3Host);
  std::cout << "initial frame id: " << initialFrameId << std::endl;
  std::cout << "thread id: " << threadId << std::endl;
  std::cout << "sfinfo sampleRate: " << sfinfo.samplerate << std::endl;
  std::cout << "readcount: " << readcount << std::endl;

  // init cafData
  CAF_DATA cafData(actorSystem);

  // Init PA
  PaStreamParameters inputParameters, outputParameters;
  PaStream *stream;
  PaError err;

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
  if (inputParameters.device == paNoDevice) {
      fprintf(stderr,"\nError: No default input device.");
      goto error;
  }
  // TODO: handle live input audio
  inputParameters.channelCount = 1;
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"\nError: No default output device.");
      goto error;
  }
  outputParameters.channelCount = audioData.sfinfo.channels;
  outputParameters.sampleFormat = PA_SAMPLE_TYPE;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
            &stream,
            &inputParameters,
            &outputParameters,
            audioData.sfinfo.samplerate,
            AUDIO_BUFFER_FRAMES,
            paNoFlag, /* paClipOn, */
            callback,
            &audioData );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  while(
      true
//          audioData.playState != 0
//            && audioData.playState != 2
//            && audioData.index > -1
//            && audioData.index < audioData.sfinfo.frames * audioData.sfinfo.channels - 1
  ) // 0: STOP, 1: PLAY, 2: PAUSE, 3: RW, 4: FF
  {
    // hold thread open until stopped

    std::cout << "start loop -- thread id: " <<  Gj::Audio::ThreadStatics::getThreadId() << std::endl;

    // here is our chance to pull data out of the actor_system through the cafData obj
    // and
    // make it accessible to our running audio callback through the audioData obj

    if (audioData.readComplete) { // reached end of input file
        Gj::Audio::ThreadStatics::setPlayState(0);
        break;
    }

    if (audioData.fadeIn > 0.001) {
        audioData.fadeIn -= 0.001;
        audioData.volume += 0.001;
    }

    std::cout << " my theradId: " << threadId << std::endl;
    std::cout << " global threadId: " << Gj::Audio::ThreadStatics::getThreadId() << std::endl;

    if ( threadId != Gj::Audio::ThreadStatics::getThreadId() ) { // fadeout, break + cleanup
        if (audioData.fadeOut < 0.001) { // break + cleanup
            break;
        } else { // continue fading out
            audioData.volume -= 0.0001;
            audioData.fadeOut -= 0.0001;
        }
    } else {
        audioData.playbackSpeed = Gj::Audio::ThreadStatics::playbackSpeed;
        audioData.playState = Gj::Audio::ThreadStatics::playState;
        Gj::Audio::ThreadStatics::setFrameId( (long) audioData.index );

        //    std::cout << "\n =========== \n";
        //    std::cout << "\n audioData.playState: " << audioData.playState << "\n";
        //    std::cout << "\n audioData.index " << audioData.index << "\n";
        //    std::cout << "\n =========== \n";
    }
  }

  if ( threadId == Gj::Audio::ThreadStatics::getThreadId() ) { // current audio thread has reached natural end of file
      if (audioData.playState == 1) {
          Gj::Audio::ThreadStatics::setPlayState(0);
      } else {
          Gj::Audio::ThreadStatics::setPlayState(audioData.playState);
      }
      Gj::Audio::ThreadStatics::setReadComplete(true);
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
    Gj::Audio::ThreadStatics::setPlayState(0);
    freeAudioData(&audioData);
    fprintf( stderr, "\nAn error occurred while using the portaudio stream" );
    fprintf( stderr, "\nError number: %d", err );
    fprintf( stderr, "\nError message: %s", Pa_GetErrorText( err ) );
    return 1;
};

} // Audio
} // Gj
