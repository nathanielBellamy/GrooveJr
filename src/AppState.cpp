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

AppState AppState::setAudioFramesPerBuffer(const AppState appState, const int audioFramesPerBuffer) {
  const AppState newState = {
    audioFramesPerBuffer,
    appState.playState,
    appState.sceneId,
    appState.sceneIndex
  };
  return newState;
}

AppState AppState::setPlayState(const AppState appState, const PlayState playState) {
  const AppState newState {
    appState.audioFramesPerBuffer,
    playState,
    appState.sceneId,
    appState.sceneIndex
  };
  return newState;
};

AppStatePacket AppState::toPacket() const {
    const AppStatePacket packet {
      audioFramesPerBuffer,
      psToInt(playState),
      sceneId
    };
    return packet;
}

AppState AppState::fromPacket(const AppStatePacket& packet) {
  const AppState appState {
      packet.audioFramesPerBuffer,
      intToPs(packet.playState),
      packet.sceneId,
      packet.sceneIndex
    };
    return appState;
};

} // Gj