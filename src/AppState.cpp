//
// Created by ns on 11/16/24.
//

#include "AppState.h"

namespace Gj {

AppState::AppState(int audioFramesPerBuffer, PlayState playState)
  : audioFramesPerBuffer(audioFramesPerBuffer)
  , playState(playState)
  {}

AppState AppState::setAudioFramesPerBuffer(const AppState appState, int audioFramesPerBuffer) {
  const AppState newState = {
    audioFramesPerBuffer,
    appState.playState
  };
  return newState;
}

AppState AppState::setPlayState(const AppState appState, Gj::PlayState playState) {
  const AppState newState { appState.audioFramesPerBuffer, playState };
  return newState;
};

AppStatePacket AppState::toPacket() const {
    AppStatePacket packet {
      audioFramesPerBuffer,
      psToInt(playState)
    };
    return packet;
}

AppState AppState::fromPacket(const AppStatePacket& packet) {
  const AppState appState {
      packet.audioFramesPerBuffer,
      intToPs(packet.playState)
    };
    return appState;
};

} // Gj