//
// Created by ns on 11/16/24.
//

#include "AppState.h"

namespace Gj {

AppState::AppState(Gj::PlayState playState)
    : playState(playState)
      {}

AppState AppState::setPlayState(AppState appState, Gj::PlayState playState) {
  AppState newState { playState };
  return newState;
};

AppStatePacket AppState::toPacket() {
    AppStatePacket packet { Gj::psToInt(playState) };
    return packet;
}

AppState AppState::fromPacket(const AppStatePacket& packet) {
    AppState appState { Gj::intToPs(packet.playState) };
    return appState;
};

} // Gj