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
    float*                           inputBuffers[2]{nullptr, nullptr};
    float*                           inputBuffersProcessHead[2]{nullptr, nullptr};
    Channel                          dryChannel;
    float                            channelCount;
    int                              effectsChannelCount;
    std::array<Effects::EffectsChannelProcessData, MAX_EFFECTS_CHANNELS> effectsChannelsProcessData{};
    float**                          effectsChannelsWriteOut[MAX_EFFECTS_CHANNELS]{};

    AudioData(
      const sf_count_t index,
      const PlayState playState,
      const float playbackSpeed,
      const float channelCount,
      const int effectsChannelCount)
        : index(index)
        , playState(playState)
        , playbackSpeed(playbackSpeed)
        , readComplete(false)
        , volume(0.0)
        , fadeIn(1.0)
        , fadeOut(1.0)
        , channelCount(channelCount)
        , effectsChannelCount(effectsChannelCount)
        {
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
        "Destroyed AudioData"
      );
    }
};

} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H