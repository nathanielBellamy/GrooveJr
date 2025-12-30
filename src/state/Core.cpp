//
// Created by ns on 11/16/24.
//

#include "Core.h"

namespace Gj {
namespace State {
Core::Core(
  const ID id,
  const jack_nframes_t audioFramesPerBuffer,
  const PlayState playState,
  const Db::Scene& scene,
  const sf_count_t crossfade)
: id(id)
  , audioFramesPerBuffer(audioFramesPerBuffer)
  , playState(playState)
  , scene(scene)
  , crossfade(crossfade) {
}

Core::Core() {
  const auto appState = fromAppStateEntity(Db::AppStateEntity::base());
  id.store(appState.id);
  audioFramesPerBuffer.store(appState.audioFramesPerBuffer);
  playState.store(appState.playState);
  scene.store(appState.scene);
  crossfade.store(appState.crossfade);
}

Packet Core::toPacket() {
  const Db::DecoratedAudioFile daf = getCurrentlyPlaying();
  AtomicStr currPlayAlbumTitle, currPlayArtistName, currPlayTrackTitle;
  if (!daf.isValid()) {
    currPlayAlbumTitle = AtomicStr("-");
    currPlayArtistName = AtomicStr("-");
    currPlayTrackTitle = AtomicStr("-");
  } else {
    currPlayAlbumTitle = daf.album.title;
    currPlayArtistName = daf.artist.name;
    currPlayTrackTitle = daf.track.title;
  }

  return {
    id.load(),
    audioFramesPerBuffer.load(),
    psToInt(playState.load()),
    scene.load().id,
    crossfade.load(),
    daf.audioFile.id,
    currPlayAlbumTitle,
    currPlayArtistName,
    currPlayTrackTitle
  };
}

Core Core::fromAppStateEntity(const Db::AppStateEntity& appStateEntity) {
  Db::Scene scene("My Scene", 1.0f);
  return {
    appStateEntity.id,
    appStateEntity.audioFramesPerBuffer,
    STOP,
    scene,
    appStateEntity.crossfade,
  };
}
} // State
} // Gj
