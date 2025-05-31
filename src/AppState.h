//
// Created by ns on 11/16/24.
//

#ifndef APPSTATE_H
#define APPSTATE_H

#include "./enums/PlayState.h"

#include "./db/entity/AppStateEntity.h"

namespace Gj {

struct AppStatePacket {
    int audioFramesPerBuffer;
    int playState;
    int sceneId;
    int sceneIndex;
};

template <class Inspector>
bool inspect(Inspector& f, AppStatePacket& x) {
    return f.object(x).fields(f.field("", x.playState));
}

struct AppState {
  int audioFramesPerBuffer;
  PlayState playState;
  int sceneId;
  int sceneIndex;

  AppState();
  AppState(int audioFramesPerBuffer, PlayState playState, int sceneId, int sceneIndex);
  AppStatePacket toPacket() const;
  // TODO?
  static AppState fromPacket(const AppStatePacket& packet);
  static AppState fromAppStateEntity(Db::AppStateEntity appStateEntity);

  // mutations
  void setFromEntity(Db::AppStateEntity appStateEntity);
  static AppState setAudioFramesPerBuffer(AppState appState, int audioFramesPerBuffer);
  static AppState setPlayState(AppState appState, PlayState playState);
};

} // Gj

#endif //APPSTATE_H
