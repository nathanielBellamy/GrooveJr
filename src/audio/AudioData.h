#ifndef GJAUDIOAUDIODATA_H
#define GJAUDIOAUDIODATA_H
#include <sndfile.hh>
#include "../enums/PlayState.h"
#include "./effects/EffectsChannelProcessData.h"
#include "./Channel.h"
#include "./Constants.h"

#include <jack/ringbuffer.h>

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
    float*                           mainInBuffers[2]{nullptr, nullptr};
    float*                           mainOutBuffers[2]{nullptr, nullptr};
    float                            channelCount;
    int                              effectsChannelCount;
    std::array<Effects::EffectsChannelProcessData, MAX_EFFECTS_CHANNELS> effectsChannelsProcessData{};
                                     // eCS[2k] = {gain channel k}, eCS[2k+1 = {pan channel k}
    float                            effectsChannelsSettings[MAX_EFFECTS_CHANNELS * 2]{};
    jack_ringbuffer_t*               effectsChannelsSettingsRB{nullptr};
    float*                           effectsChannelsWriteOut[MAX_EFFECTS_CHANNELS][2]{};

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
        "Audio::AudioData::AudioData",
        "Instantiated AudioData"
      );
    }

    ~AudioData() {
      Logging::write(
        Info,
        "Audio::AudioData::~AudioData",
        "Destroyed AudioData"
      );
    }
};

} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H