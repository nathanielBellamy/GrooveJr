#ifndef AUDIO_DATA_H
#define AUDIO_DATA_H
#include <sndfile.hh>
#include "./effects/vst3/Plugin.h"
#include "../enums/PlayState.h"

struct AUDIO_DATA {
    sf_count_t                       index;
    float                            *buffer;
    SNDFILE                          *file;
    SF_INFO                          sfinfo;
    long                             readcount;
    Gj::PlayState                    playState;
    float                            playbackSpeed;
    bool                             readComplete;
    float                            volume;
    float                            fadeIn;
    float                            fadeOut;
    std::vector<Gj::Audio::Effects::Vst3::Plugin*>& vst3Plugins;

    AUDIO_DATA(float* buffer, SNDFILE* file, SF_INFO sfinfo, sf_count_t index, long readcount, Gj::PlayState playState, std::vector<Gj::Audio::Effects::Vst3::Plugin*>& vst3Plugins) :
          buffer(buffer)
        , file(file)
        , sfinfo(sfinfo)
        , index(index)
        , readcount(readcount)
        , playState(playState)
        , readComplete(false)
        , volume(0.0)
        , fadeIn(1.0)
        , fadeOut(1.0)
        , vst3Plugins(vst3Plugins)
        {}
};

#endif