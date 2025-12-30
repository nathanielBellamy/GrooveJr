//
// Created by ns on 12/30/25.
//

#ifndef GJSTATEPACKET_H
#define GJSTATEPACKET_H

#include <sndfile.h>
#include <jack/jack.h>

#include "../enums/PlayState.h"

#include "../db/entity/AppStateEntity.h"
#include "../db/entity/mixer/Scene.h"
#include "../db/dto/musicLibrary/DecoratedAudioFile.h"
#include "../types/Types.h"
#include "../types/AtomicStr.h"

namespace Gj {
namespace State {
struct Packet {
  ID id;
  jack_nframes_t audioFramesPerBuffer;
  int playState;
  ID sceneId;
  sf_count_t crossfade;
  ID currentlyPlayingId;
  AtomicStr currentlyPlayingAlbumTitle;
  AtomicStr currentlyPlayingArtistName;
  AtomicStr currentlyPlayingTrackTitle;
};

template<class Inspector>
bool inspect(Inspector& f, State::Packet& x) {
  return f.object(x).fields(f.field("", x.playState));
}
} // State
} // Gj

#endif //GJSTATEPACKET_H
