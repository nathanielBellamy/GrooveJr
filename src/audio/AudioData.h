#ifndef GJAUDIOAUDIODATA_H
#define GJAUDIOAUDIODATA_H
#include <sndfile.hh>
#include "../enums/PlayState.h"
#include "./effects/EffectsChannelProcessData.h"
#include "./Channel.h"

namespace Gj {
namespace Audio {

class AudioData {
  public:
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
    std::array<Effects::EffectsChannelProcessData, 100> effectsChannelsProcessData;
    std::array<float**, 100>         effectsChannelsWriteOut;
    int32_t                          framesPerBuffer_32_t;
    int64_t                          framesPerBuffer_64_t;


    AudioData(){};

    AudioData(sf_count_t index, PlayState playState, Mixer* mixer)
        : index(index)
        , playState(playState)
        , readComplete(false)
        , volume(0.0)
        , fadeIn(1.0)
        , fadeOut(1.0)
        , mixer(mixer)
    {}
};

} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H