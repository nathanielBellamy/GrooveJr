#ifndef GJAUDIOAUDIODATA_H
#define GJAUDIOAUDIODATA_H
#include <sndfile.hh>
#include "./Mixer.h"
#include "../enums/PlayState.h"

namespace Gj {
namespace Audio {

class AudioData {
  public:
    sf_count_t                       index;
    float                            *buffer;
    SNDFILE                          *file;
    SF_INFO                          sfinfo;
    long                             readcount;
    PlayState                        playState;
    float                            playbackSpeed;
    bool                             readComplete;
    float                            volume;
    float                            fadeIn;
    float                            fadeOut;
    Mixer*                           mixer;

    AudioData(){};

    AudioData(float* buffer, SNDFILE* file, SF_INFO sfinfo, sf_count_t index, long readcount, PlayState playState, Mixer* mixer)
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

} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H