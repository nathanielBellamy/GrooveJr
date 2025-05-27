//
// Created by ns on 11/16/24.
//

#include "AppState.h"

namespace Gj {

AppState::AppState(const int audioFramesPerBuffer, const PlayState playState, const int sceneId)
  : audioFramesPerBuffer(audioFramesPerBuffer)
  , playState(playState)
  , sceneId(sceneId)
  {}

AppState AppState::setAudioFramesPerBuffer(const AppState appState, const int audioFramesPerBuffer) {
  const AppState newState = {
    audioFramesPerBuffer,
    appState.playState,
    appState.sceneId
  };
  return newState;
}

AppState AppState::setPlayState(const AppState appState, const PlayState playState) {
  const AppState newState { appState.audioFramesPerBuffer, playState, appState.sceneId };
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
      packet.sceneId
    };
    return appState;
};

} // Gj