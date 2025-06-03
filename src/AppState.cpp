//
// Created by ns on 11/16/24.
//

#include "AppState.h"

#include "actors/AudioThread.h"

namespace Gj {

AppState::AppState(const int id, const int audioFramesPerBuffer, const PlayState playState, const int sceneId, const int sceneIndex)
  : id(id)
  , audioFramesPerBuffer(audioFramesPerBuffer)
  , playState(playState)
  , sceneId(sceneId)
  , sceneIndex(sceneIndex)
  {}

AppState::AppState() {
  const auto appState = fromAppStateEntity(Db::AppStateEntity::base());
  id.store(appState.id);
  audioFramesPerBuffer.store( appState.audioFramesPerBuffer);
  playState.store( appState.playState);
  sceneId.store( appState.sceneId);
  sceneIndex.store(appState.sceneIndex);
}

AppStatePacket AppState::toPacket() const {
    const AppStatePacket packet {
      id.load(),
      audioFramesPerBuffer.load(),
      psToInt(playState.load()),
      sceneId.load(),
    };
    return packet;
}

AppState AppState::fromAppStateEntity(const Db::AppStateEntity appStateEntity) {
  return {
    appStateEntity.id,
    appStateEntity.audioFramesPerBuffer,
    STOP,
    appStateEntity.sceneId,
    appStateEntity.sceneIndex
  };
}

} // Gj