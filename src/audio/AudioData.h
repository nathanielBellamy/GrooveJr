#ifndef GJAUDIOAUDIODATA_H
#define GJAUDIOAUDIODATA_H
#include <sndfile.hh>
#include "../enums/PlayState.h"
#include "./effects/EffectsChannelProcessData.h"
#include "./Channel.h"
#include "./Constants.h"

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
    Channel*                         dryChannel;
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
      float*** effectsChannelWriteOutBuffer)
        : index(index)
        , playState(playState)
        , readComplete(false)
        , volume(0.0)
        , fadeIn(1.0)
        , fadeOut(1.0)
        , inputBuffers(inputBuffers)
        , inputBuffersProcessHead(inputBuffers)
        , buffersA(buffersA)
        , buffersB(buffersB)
        {

      inputBuffersProcessHead[0] = inputBuffers[0] + index;
      inputBuffersProcessHead[1] = inputBuffers[1] + index;

      for (int i = 0; i < MAX_EFFECTS_CHANNELS; i++) {
        effectsChannelsWriteOut[i] = effectsChannelWriteOutBuffer[i];
      }
    }
};

} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H