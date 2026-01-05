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
#include "mixer/Packet.h"

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
  Mixer::Packet mixerPacket{};
};

template<class Inspector>
bool inspect(Inspector& f, Packet& x) {
  return f.object(x).fields(
    f.field("id", x.id),
    f.field("audioFramesPerBuffer", x.audioFramesPerBuffer),
    f.field("playState", x.playState),
    f.field("sceneId", x.sceneId),
    f.field("crossfade", x.crossfade),
    f.field("currentlyPlayingId", x.currentlyPlayingId),
    f.field("currentlyPlayingAlbumTitle", x.currentlyPlayingAlbumTitle),
    f.field("currentlyPlayingArtistName", x.currentlyPlayingArtistName),
    f.field("currentlyPlayingTrackTitle", x.currentlyPlayingTrackTitle),
    f.field("mixerPacket", x.mixerPacket)
  );
}
} // State
} // Gj

#endif //GJSTATEPACKET_H
