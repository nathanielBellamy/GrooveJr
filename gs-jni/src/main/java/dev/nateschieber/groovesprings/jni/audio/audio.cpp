#include <string>
#include <unistd.h>
#include <iostream>
#include <sndfile.hh>
#include <portaudio.h>
#include "jni.h"
#include "./audio.h"
#include "./audio_data.h"
#include "./constants.h"
#include "./jni_data.h"

#define PA_SAMPLE_TYPE      paFloat32

typedef float SAMPLE;

Audio::Audio(JNIEnv* env, jstring jFileName)
{
    char const* fileName;
    fileName = env->GetStringUTFChars(jFileName, 0);

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }
    std::cout << "\n audio.cpp has file: " << fileName;
};

void Audio::freeAudioData(AUDIO_DATA *audioData) {
  // printf("\nCleaning up resources...");

  free(audioData->buffer);
  sf_close(audioData->file);

  // printf("\nDone.");
};

int Audio::init_pa(AUDIO_DATA *audioData)
{
  audioData->index = 0;

  // https://svn.ict.usc.edu/svn_vh_public/trunk/lib/vhcl/libsndfile/doc/api.html
  // > When opening a file for read, the format field should be set to zero before calling sf_open().
  audioData->sfinfo.format = 0;

  if (! (audioData->file = sf_open("Unkown Artist/Unkown Album/test.mp3", SFM_READ, &audioData->sfinfo)))
  {
		printf ("Not able to open input file.\n") ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
    return 1 ;
  };

  // Allocate memory for data
  audioData->buffer = (float *) malloc(audioData->sfinfo.frames * audioData->sfinfo.channels * sizeof(float));
  if (!audioData->buffer) {
      printf("\nCannot allocate memory");
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(audioData->file, audioData->buffer, audioData->sfinfo.frames * audioData->sfinfo.channels);
  if (readcount == 0) {
      printf("\nCannot read file");
      return 1;
  }

  return 0;
};

int Audio::init_jni(AUDIO_DATA *audioData) {
  // get necessary objects from JNI
  JNI_DATA jniData;
  jniData.gsPlayback = this->env->FindClass("dev/nateschieber/groovesprings/actors/GsPlaybackThread");
  jniData.setCurrFrameId = this->env->GetStaticMethodID (jniData.gsPlayback, "setCurrFrameId", "(Ljava/lang/Long;)V");
  jniData.getStopped = this->env->GetStaticMethodID (jniData.gsPlayback, "getStopped", "()Z");

  jniData.jNum = this->env->FindClass("java/lang/Long");
  jniData.jNumInit = this->env->GetMethodID(jniData.jNum, "<init>", "(J)V");

  audioData->jniData = &jniData;

  return 0;
}

// portaudio callback
// do not allocate/free memory within this method
// as it may be called at system-interrupt level
int Audio::callback(const void *inputBuffer, void *outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void *userData )
{
  SAMPLE *out = (SAMPLE*)outputBuffer;
  unsigned int i;
  (void) inputBuffer;
  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  AUDIO_DATA *audioData = (AUDIO_DATA *) userData;

  // TODO: replace stopped with playState, add pause functionality
  bool stopped;
  stopped = audioData->jniData->env->CallStaticBooleanMethod(
        audioData->jniData->gsPlayback,
        audioData->jniData->getStopped
  );

  if( audioData->buffer == NULL )
  {
      for( i=0; i < framesPerBuffer; i++ )
      {
          *out++ = 0;  /* left - silent */
          *out++ = 0;  /* right - silent */
      }
  }
  else if (audioData->index > audioData->sfinfo.frames * audioData->sfinfo.channels + 1)
  {
    // audioData->index = 0;
    // return paComplete;
    return 1;
  }
  else if (stopped) {
    return 1;
  }
  else
  {
    // audioData->buffer --> paOut
    for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
      *out++ = audioData->buffer[audioData->index + i];
      audioData->index++;
    }
  }

  if (audioData->index % 1000 == 0)
  {
    Audio::jSetCurrFrameId(audioData->jniData, audioData->index);
  };

  return paContinue;
};

int Audio::run()
{
  AUDIO_DATA audioData;

  if ( init_jni(&audioData) != 0)
  {
    return 1;
  };

  if ( init_pa(&audioData) != 0)
  {
    freeAudioData(&audioData);
    return 1;
  };

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
  inputParameters.channelCount = 2;       /* stereo in from file */
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"\nError: No default output device.");
      goto error;
  }
  outputParameters.channelCount = 2;       /* stereo output */
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

  char c;
  while(
    env->CallStaticBooleanMethod(
        audioData.jniData->gsPlayback,
        audioData.jniData->getStopped
    ) == false
  ) {}

  err = Pa_StopStream( stream );
  if( err != paNoError ) goto error;

  err = Pa_CloseStream( stream );
  if( err != paNoError ) goto error;
  Pa_Terminate();
  return 0;

  error:
    Pa_Terminate();
    fprintf( stderr, "\nAn error occurred while using the portaudio stream" );
    fprintf( stderr, "\nError number: %d", err );
    fprintf( stderr, "\nError message: %s", Pa_GetErrorText( err ) );
    return 1;
};

void Audio::jSetCurrFrameId(
    JNI_DATA* jniData,
    int currFrameId
){
   jobject jCurrFrameId = jniData->env->NewObject(
        jniData->jNum,
        jniData->jNumInit,
        currFrameId
   );
   jniData->env->CallVoidMethod(
        jniData->gsPlayback,
        jniData->setCurrFrameId,
        jCurrFrameId
   );
}
