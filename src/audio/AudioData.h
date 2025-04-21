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
    float**                          inputBuffers;
    float**                          inputBuffersProcessHead;
    float**                          buffersA;
    float**                          buffersB;
    Channel                          dryChannel;
    float                            channelCount;
    int                              effectsChannelCount;
    std::array<Effects::EffectsChannelProcessData, MAX_EFFECTS_CHANNELS> effectsChannelsProcessData{};
    std::array<float**, MAX_EFFECTS_CHANNELS> effectsChannelsWriteOut{};
    int32_t                          framesPerBuffer_32_t;
    int64_t                          framesPerBuffer_64_t;

    AudioData(){};

    AudioData(
      sf_count_t index,
      PlayState playState,
      float** inputBuffers,
      float** buffersA,
      float** buffersB,
      float*** effectsChannelsWriteOutBuffer)
        : index(index)
        , playState(playState)
        , readComplete(false)
        , volume(0.0)
        , fadeIn(1.0)
        , fadeOut(1.0)
        , inputBuffers(inputBuffers)
        , inputBuffersProcessHead(new float*[2])
        , buffersA(buffersA)
        , buffersB(buffersB)
        {

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
        // TODO: debug access effectsChannelsWriteOutBuffer
        // effectsChannelsWriteOut[i] = new float*[2];
        // effectsChannelsWriteOut[i][0] = new float[512];
        // effectsChannelsWriteOut[i][1] = new float[512];
        // if (effectsChannelsWriteOutBuffer[i] == nullptr) {
        //   Logging::write(
        //     Error,
        //     "AudioData::AudioData",
        //     "effectsChannelsWriteOutBuffer null row: " + std::to_string(i)
        //   );
        // } else {
        //   // effectsChannelsWriteOut[i] = effectsChannelsWriteOutBuffer[i];
        // }
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

      delete[] inputBuffersProcessHead;

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