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

#include "./AudioData.h"
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
  std::shared_ptr<JackClient> jackClient;
  bool jackClientIsActive;
  float* buffer{};
  SF_INFO sfInfo;
  SNDFILE* file{};
  float* inputBuffer{};
  float* inputBuffers[2]{}; // full song audio data
  float* inputBuffersProcessHead[2]{}; // where jack process callback should start
  float buffersA[2 * MAX_AUDIO_FRAMES_PER_BUFFER]{};
  float buffersB[2 * MAX_AUDIO_FRAMES_PER_BUFFER]{};
  float** buffersAPtr;
  float** buffersBPtr;
  float* effectsChannelsWriteOutBuffer;
  float effectsChannelsSettings[2 * MAX_EFFECTS_CHANNELS]{};
  sf_count_t playbackSettingsToAudioThread[PlaybackSettingsToAudioThread_Count]{};
  sf_count_t playbackSettingsFromAudioThread[PlaybackSettingsFromAudioThread_Count]{};
  AudioData audioData;

  int setupAudioData();
  IAudioClient::Buffers getPluginBuffers(const Effects::EffectsChannel* effectsChannel, int channelIdx, int pluginIdx, const AudioData &audioData) const;

  bool allocateInputBuffers();
  bool populateInputBuffers() const;
  bool setupInputBuffers();

  bool allocateProcessBuffers();
  [[nodiscard]]
  bool deleteBuffers() const;

  int updateAudioDataFromMixer(
    jack_ringbuffer_t* effectsChannelsSettingsRB,
    jack_ringbuffer_t* playbackSettingsToAudioThreadRB,
    jack_ringbuffer_t* playbackSettingsFromAudioThreadRB,
    int channelCount
  );

  public:
    Cassette(
        actor_system& actorSystem,
        AppState* gAppState,
        Mixer* mixer
    );
    ~Cassette();

    int play();
    void cleanup();
};

} // Audio
} // Gj

#endif