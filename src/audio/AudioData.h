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
                                     // eCS[4k]   = {factorLL channel k}
                                     // eCS[4k+1] = {factorLR channel k}
                                     // eCS[4k+2] = {factorRL channel k}
                                     // eCS[4k+3] = {factorRR channel k}
    float                            effectsChannelsSettings[MAX_EFFECTS_CHANNELS * 4]{};
    jack_ringbuffer_t*               effectsChannelsSettingsRB{nullptr};
    jack_ringbuffer_data_t*          effectsChannelsSettingsReadVector[2]{nullptr, nullptr};
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

    static float factorLL(
      const float gain,
      const float mute,
      const float solo,
      const float pan,
      const float soloEngaged,
      const float channelCount
      ) {

      const float panL = (pan - 1.0f) / 2.0f;
      return (1.0f - mute) * panL * gain / channelCount;
    }

    static float factorLR(
      const float gain,
      const float mute,
      const float solo,
      const float pan,
      const float soloEngaged,
      const float channelCount
      ) {

      const float panL = (pan - 1.0f) / 2.0f;
      return (1.0f - mute) * panL * gain / channelCount;
    }

    static float factorRL(
      const float gain,
      const float mute,
      const float solo,
      const float pan,
      const float soloEngaged,
      const float channelCount
      ) {

      const float panR = (pan + 1.0f) / 2.0f;
      return (1.0f - mute) * panR * gain / channelCount;
    }

    static float factorRR(
      const float gain,
      const float mute,
      const float solo,
      const float pan,
      const float soloEngaged,
      const float channelCount
      ) {

      const float panR = (pan + 1.0f) / 2.0f;
      return (1.0f - mute) * panR * gain / channelCount;
    }
};

} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H