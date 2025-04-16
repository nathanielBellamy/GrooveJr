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
    PlayState                        playState;
    float                            playbackSpeed;
    bool                             readComplete;
    float                            volume;
    float                            fadeIn;
    float                            fadeOut;
    Mixer*                           mixer;

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