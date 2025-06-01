//
// Created by ns on 11/16/24.
//

#include "AppState.h"

namespace Gj {

AppState::AppState(const int audioFramesPerBuffer, const PlayState playState, const int sceneId, const int sceneIndex)
  : audioFramesPerBuffer(audioFramesPerBuffer)
  , playState(playState)
  , sceneId(sceneId)
  , sceneIndex(sceneIndex)
  {}

AppState::AppState() {
  const auto appState = fromAppStateEntity(Db::AppStateEntity::base());
  audioFramesPerBuffer.store( appState.audioFramesPerBuffer);
  playState.store( appState.playState);
  sceneId.store( appState.sceneId);
  sceneIndex.store(appState.sceneIndex);
}

void AppState::setFromEntity(const Db::AppStateEntity appStateEntity) {
  audioFramesPerBuffer.store(appStateEntity.audioFramesPerBuffer);
  playState.store( STOP);
  sceneId.store(appStateEntity.sceneId);
  sceneIndex.store(appStateEntity.sceneIndex);
}


AppStatePacket AppState::toPacket() const {
    const AppStatePacket packet {
      audioFramesPerBuffer.load(),
      psToInt(playState.load()),
      sceneId.load(),
    };
    return packet;
}

AppState AppState::fromAppStateEntity(const Db::AppStateEntity appStateEntity) {
  return AppState(
    appStateEntity.audioFramesPerBuffer,
    STOP,
    appStateEntity.sceneId,
    appStateEntity.sceneIndex
  );
}


} // Gj