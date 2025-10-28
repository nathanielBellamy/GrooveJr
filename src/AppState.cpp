//
// Created by ns on 11/16/24.
//

#include "AppState.h"

namespace Gj {

AppState::AppState(
  const int id,
  const jack_nframes_t audioFramesPerBuffer,
  const PlayState playState,
  const int sceneId,
  const int sceneIndex,
  const sf_count_t crossfade)
  : id(id)
  , audioFramesPerBuffer(audioFramesPerBuffer)
  , playState(playState)
  , sceneId(sceneId)
  , sceneIndex(sceneIndex)
  , crossfade(crossfade)
  {}

AppState::AppState() {
  const auto appState = fromAppStateEntity(Db::AppStateEntity::base());
  id.store(appState.id);
  audioFramesPerBuffer.store( appState.audioFramesPerBuffer);
  playState.store( appState.playState);
  sceneId.store( appState.sceneId);
  sceneIndex.store(appState.sceneIndex);
  crossfade.store( appState.crossfade);
}

AppStatePacket AppState::toPacket() {
  const Db::DecoratedAudioFile daf = getCurrentlyPlaying();
  std::string currPlayAlbumTitle, currPlayArtistName, currPlayTrackTitle;
  if (!daf.isValid()) {
    currPlayAlbumTitle = "-";
    currPlayArtistName = "-";
    currPlayTrackTitle = "-";
  } else {
    currPlayAlbumTitle = daf.album.title;
    currPlayArtistName = daf.artist.name;
    currPlayTrackTitle = daf.track.title;
  }

  const AppStatePacket packet {
    id.load(),
    audioFramesPerBuffer.load(),
    psToInt(playState.load()),
    sceneId.load(),
    sceneIndex.load(),
    crossfade.load(),
    currPlayAlbumTitle,
    currPlayArtistName,
    currPlayTrackTitle
  };
  return packet;
}

AppState AppState::fromAppStateEntity(const Db::AppStateEntity appStateEntity) {
  return {
    appStateEntity.id,
    appStateEntity.audioFramesPerBuffer,
    STOP,
    appStateEntity.sceneId,
    appStateEntity.sceneIndex,
    appStateEntity.crossfade
  };
}

} // Gj