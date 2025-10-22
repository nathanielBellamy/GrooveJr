//
// Created by ns on 11/16/24.
//

#ifndef APPSTATE_H
#define APPSTATE_H

#include <atomic>
#include <sndfile.h>
#include <string>

#include <jack/jack.h>

#include "db/dto/musicLibrary/DecoratedAudioFile.h"
#include "./enums/PlayState.h"

#include "./db/entity/AppStateEntity.h"

namespace Gj {

struct AppStatePacket {
    int id;
    jack_nframes_t audioFramesPerBuffer;
    int playState;
    int sceneId;
    int sceneIndex;
    sf_count_t crossfade;
};

template <class Inspector>
bool inspect(Inspector& f, AppStatePacket& x) {
    return f.object(x).fields(f.field("", x.playState));
}

struct AppState {
  std::atomic<int> id{};
  std::atomic<jack_nframes_t> audioFramesPerBuffer{};
  std::atomic<PlayState> playState{};
  std::atomic<int> sceneId{};
  std::atomic<int> sceneIndex{};
  std::atomic<sf_count_t> crossfade{ 0 };
  std::atomic<bool> queuePlay = false;
  std::atomic<Db::ID> queueIndex = 0;

  AppState();
  AppState(
    int id,
    jack_nframes_t audioFramesPerBuffer,
    PlayState playState,
    int sceneId,
    int sceneIndex,
    sf_count_t crossfade
  );
  AppStatePacket toPacket() const;
  static AppState fromAppStateEntity(Db::AppStateEntity appStateEntity);

  // mutations
  void setFromEntity(const Db::AppStateEntity appStateEntity) {
    id.store(appStateEntity.id);
    audioFramesPerBuffer.store(appStateEntity.audioFramesPerBuffer);
    playState.store( STOP);
    sceneId.store(appStateEntity.sceneId);
    sceneIndex.store(appStateEntity.sceneIndex);
    crossfade.store(appStateEntity.crossfade);
  };

  jack_nframes_t getAudioFramesPerBuffer() const {
    return audioFramesPerBuffer.load();
  };
  void setAudioFramesPerBuffer(const jack_nframes_t val) {
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

  sf_count_t getCrossfade() const {
    return crossfade.load();
  }
  void setCrossfade(const sf_count_t val) {
    crossfade.store(val);
  }

  std::string toString() const {
    return " id: " + std::to_string(id) + " audioFramesPerBuffer: " + std::to_string(audioFramesPerBuffer) + " playState: " + std::to_string(playState) + " sceneId: " + std::to_string(sceneId) + " sceneIndex: " + std::to_string(sceneIndex);
  };
};

} // Gj

#endif //APPSTATE_H
