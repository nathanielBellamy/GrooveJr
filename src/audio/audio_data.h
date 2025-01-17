#ifndef AUDIO_DATA_H
#define AUDIO_DATA_H
#include <sndfile.hh>
#include "./Mixer.h"
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
    Gj::Audio::Mixer&                mixer;

    AUDIO_DATA(float* buffer, SNDFILE* file, SF_INFO sfinfo, sf_count_t index, long readcount, Gj::PlayState playState, Gj::Audio::Mixer& mixer)
        : index(index)
        , buffer(buffer)
        , file(file)
        , sfinfo(sfinfo)
        , readcount(readcount)
        , playState(playState)
        , readComplete(false)
        , volume(0.0)
        , fadeIn(1.0)
        , fadeOut(1.0)
        , mixer(mixer)
        {}
};

#endif