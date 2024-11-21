//
// Created by ns on 11/16/24.
//

#include "AppState.h"

namespace Gs {

AppState::AppState(Gs::PlayState playState)
    : playState(playState)
      {}

AppState AppState::setPlayState(AppState appState, Gs::PlayState playState) {
  AppState newState { playState };
  return newState;
};

AppStatePacket AppState::toPacket() {
    AppStatePacket packet { Gs::psToInt(playState) };
    return packet;
}

AppState AppState::fromPacket(const AppStatePacket& packet) {
    AppState appState { Gs::intToPs(packet.playState) };
    return appState;
};

} // Gs