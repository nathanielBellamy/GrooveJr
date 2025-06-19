#ifndef GJAUDIOAUDIODATA_H
#define GJAUDIOAUDIODATA_H
#include <sndfile.hh>
#include "../enums/PlayState.h"
#include "./effects/EffectsChannelProcessData.h"
#include "./Constants.h"

#include <jack/ringbuffer.h>

#include "../Logging.h"

namespace Gj {
namespace Audio {

struct AudioData {
    sf_count_t                       readCount;
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
    sf_count_t                       playbackSettingsToAudioThread[2]{};
    jack_ringbuffer_t*               playbackSettingsToAudioThreadRB{nullptr};
                                     // pSTATRB[0] = userSettingFrameId bool
                                     // pSTATRB[1] = newFrameId sf_count_t
    sf_count_t                       playbackSettingsFromAudioThread[2]{};
    jack_ringbuffer_t*               playbackSettingsFromAudioThreadRB{nullptr};
                                     // pSFATRB[0] = doneSettingFrameId bool
                                     // pSFATRB[1] = frameId sf_count_t
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
      const float gainL,
      const float gainR,
      const float mute,
      const float muteL,
      const float muteR,
      const float solo,
      const float soloL,
      const float soloR,
      const float pan,
      const float panL,
      const float panR,
      const float channelCount
      ) {

      // const float panVal = pan <= 0.0f ? 1.0f : 1.0f - pan;
      const float panLVal = (1.0f - panL) / 2.0f;
      const float soloVal = solo == 1.0f
                              ? 1.0f
                              : soloL;
      return soloVal * (1.0f - mute) * (1.0f - muteL) * panLVal * gain * gainL / channelCount;
    }

    static float factorLR(
      const float gain,
      const float gainL,
      const float gainR,
      const float mute,
      const float muteL,
      const float muteR,
      const float solo,
      const float soloL,
      const float soloR,
      const float pan,
      const float panL,
      const float panR,
      const float channelCount
      ) {

      // const float panVal = panL >= 0.0f ? pan : 0.0;
      const float panLVal = (1.0f + panL) / 2.0f;
      const float soloVal = solo == 1.0f
                              ? 1.0f
                              : soloL;
      return soloVal * (1.0f - mute) * (1.0f - muteL) * panLVal * gain * gainL / channelCount;
    }

    static float factorRL(
      const float gain,
      const float gainL,
      const float gainR,
      const float mute,
      const float muteL,
      const float muteR,
      const float solo,
      const float soloL,
      const float soloR,
      const float pan,
      const float panL,
      const float panR,
      const float channelCount
      ) {

      // const float panVal = pan <= 0.0f ? -pan : 0.0f;
      const float panRVal = (1.0f - panR) / 2.0f;
      const float soloVal = solo == 1.0f
                              ? 1.0f
                              : soloR;
      return soloVal * (1.0f - mute) * (1.0f - muteR) * panRVal * gain * gainR / channelCount;
    }

    static float factorRR(
      const float gain,
      const float gainL,
      const float gainR,
      const float mute,
      const float muteL,
      const float muteR,
      const float solo,
      const float soloL,
      const float soloR,
      const float pan,
      const float panL,
      const float panR,
      const float channelCount
      ) {

      // const float panVal = pan >= 0.0f ? 1.0f : 1.0f + pan;
      const float panRVal = (1.0f + panR) / 2.0f;
      const float soloVal = solo == 1.0f
                              ? 1.0f
                              : soloR;
      return soloVal * (1.0f - mute) * (1.0f - muteR) * panRVal * gain * gainR / channelCount;
    }
};

} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H