//
// Created by ns on 11/16/24.
//

#ifndef APPSTATE_H
#define APPSTATE_H

#include <atomic>

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
  std::atomic<int> audioFramesPerBuffer;
  std::atomic<PlayState> playState;
  std::atomic<int> sceneId;
  std::atomic<int> sceneIndex;

  AppState();
  AppState(int audioFramesPerBuffer, PlayState playState, int sceneId, int sceneIndex);
  AppStatePacket toPacket() const;
  static AppState fromAppStateEntity(Db::AppStateEntity appStateEntity);

  // mutations
  void setFromEntity(Db::AppStateEntity appStateEntity);

  int getAudioFramesPerBuffer() const {
    return audioFramesPerBuffer.load();
  };
  void setAudioFramesPerBuffer(const int val) {
    if (val < 0)
      return;
    audioFramesPerBuffer.store(val);
  }

  PlayState getPlayState() const {
    return playState.load();
  };
  void setPlayState(const PlayState val) {
    playState.store(val);
  };

  int getSceneId() const {
    return sceneId.load();
  };
  void setSceneId(const int val) {
    sceneId.store(val);
  };

  int getSceneIndex() const {
    return sceneIndex.load();
  };
  void setSceneIndex(const int val) {
    sceneIndex.store(val);
  }
};

} // Gj

#endif //APPSTATE_H
