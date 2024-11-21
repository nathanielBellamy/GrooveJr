//
// Created by ns on 11/16/24.
//

#ifndef APPSTATE_H
#define APPSTATE_H

#include "./enums/PlayState.h"

namespace Gs {

struct AppStatePacket {
    int playState;
};

template <class Inspector>
bool inspect(Inspector& f, AppStatePacket& x) {
    return f.object(x).fields(f.field("", x.playState));
}

class AppState {

  public:
    AppState(Gs::PlayState playState);
    Gs::PlayState playState;

    AppStatePacket toPacket();
     // TODO?
    static AppState fromPacket(const AppStatePacket& packet);

    // mutations
    static AppState setPlayState(AppState appState, Gs::PlayState playState);
};

} // Gs

#endif //APPSTATE_H
