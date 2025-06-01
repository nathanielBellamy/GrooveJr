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

void AppState::setAudioFramesPerBuffer(const int val) {
  if (val < 0)
    return;
  audioFramesPerBuffer.store(val);
}

void AppState::setPlayState(const PlayState val) {
  playState.store(val);
};

void AppState::setSceneId(const int val) {
  sceneId.store(val);
}

void AppState::setSceneIndex(const int val) {
  sceneIndex.store(val);
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