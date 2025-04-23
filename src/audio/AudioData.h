#ifndef GJAUDIOAUDIODATA_H
#define GJAUDIOAUDIODATA_H
#include <sndfile.hh>
#include "../enums/PlayState.h"
#include "./effects/EffectsChannelProcessData.h"
#include "./Channel.h"
#include "./Constants.h"

#include "../Logging.h"

namespace Gj {
namespace Audio {

struct AudioData {
    sf_count_t                       index;
    PlayState                        playState;
    float                            playbackSpeed;
    bool                             readComplete;
    float                            volume;
    float                            fadeIn;
    float                            fadeOut;
    float*                           inputBuffers[2]{};
    float*                           inputBuffersProcessHead[2]{};
    Channel                          dryChannel;
    float                            channelCount;
    int                              effectsChannelCount;
    std::array<Effects::EffectsChannelProcessData, MAX_EFFECTS_CHANNELS> effectsChannelsProcessData{};
    float**                          effectsChannelsWriteOut[MAX_EFFECTS_CHANNELS]{};
    int32_t                          framesPerBuffer_32_t;
    int64_t                          framesPerBuffer_64_t;

    AudioData(){};

    AudioData(
      const sf_count_t index,
      const PlayState playState,
      const float playbackSpeed,
      const float channelCount,
      const int effectsChannelCount,
      float* inputBuffersIn[2],
      float* effectsChannelsWriteOutBuffer,
      const int32_t framesPerBuffer_32_t,
      const int64_t framesPerBuffer_64_t)
        : index(index)
        , playState(playState)
        , playbackSpeed(playbackSpeed)
        , readComplete(false)
        , volume(0.0)
        , fadeIn(1.0)
        , fadeOut(1.0)
        , channelCount(channelCount)
        , effectsChannelCount(effectsChannelCount)
        , framesPerBuffer_32_t(framesPerBuffer_32_t)
        , framesPerBuffer_64_t(framesPerBuffer_64_t)
        {
      inputBuffers[0] = inputBuffersIn[0];
      inputBuffers[1] = inputBuffersIn[1];

      Logging::write(
        Info,
        "AudioData::AudioData",
        "Instantiating AudioData"
      );

      inputBuffersProcessHead[0] = inputBuffers[0] + index;
      inputBuffersProcessHead[1] = inputBuffers[1] + index;

      Logging::write(
        Info,
        "AudioData::AudioData",
        "Set inputBuffersProcessHead"
      );

      for (int i = 0; i < MAX_EFFECTS_CHANNELS; i++) {
        float* effectsChannelWriteOutPre[2];
        effectsChannelWriteOutPre[0] = effectsChannelsWriteOutBuffer + (2 * i * MAX_AUDIO_FRAMES_PER_BUFFER);
        effectsChannelWriteOutPre[1] = effectsChannelsWriteOutBuffer + ((2 * i + 1) * MAX_AUDIO_FRAMES_PER_BUFFER);
        effectsChannelsWriteOut[i] = effectsChannelWriteOutPre;
      }

      Logging::write(
        Info,
        "AudioData::AudioData",
        "Instantiated AudioData"
      );
    }

    ~AudioData() {
      Logging::write(
        Info,
        "AudioData::~AudioData",
        "Destroying AudioData"
      );


      Logging::write(
        Info,
        "AudioData::~AudioData",
        "Destroyed AudioData"
      );
    }
};

} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H