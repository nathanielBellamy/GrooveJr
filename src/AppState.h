//
// Created by ns on 11/16/24.
//

#ifndef APPSTATE_H
#define APPSTATE_H

#include <atomic>
#include <string>

#include <sndfile.h>
#include <jack/jack.h>

#include "./enums/PlayState.h"

#include "./db/entity/AppStateEntity.h"
#include "./db/entity/mixer/Scene.h"
#include "./db/dto/musicLibrary/DecoratedAudioFile.h"
#include "./db/Types.h"
#include "types/AtomicStr.h"

namespace Gj {

struct AppStatePacket {
    Db::ID id;
    jack_nframes_t audioFramesPerBuffer;
    int playState;
    Db::ID sceneId;
    sf_count_t crossfade;
    Db::ID currentlyPlayingId;
    AtomicStr currentlyPlayingAlbumTitle;
    AtomicStr currentlyPlayingArtistName;
    AtomicStr currentlyPlayingTrackTitle;
};

template <class Inspector>
bool inspect(Inspector& f, AppStatePacket& x) {
    return f.object(x).fields(f.field("", x.playState));
}

struct AppState {
  std::atomic<Db::ID> id;
  std::atomic<jack_nframes_t> audioFramesPerBuffer;
  std::atomic<PlayState> playState;
  std::atomic<Db::Scene> scene;
  std::atomic<sf_count_t> crossfade{ 0 };
  std::atomic<Db::DecoratedAudioFile> currentlyPlaying;
  std::atomic<bool> queuePlay = false;
  std::atomic<Db::TrackNumber> queueIndex = 0;

  AppState();
  AppState(
    Db::ID id,
    jack_nframes_t audioFramesPerBuffer,
    PlayState playState,
    const Db::Scene& scene,
    sf_count_t crossfade
  );
  AppStatePacket toPacket();
  static AppState fromAppStateEntity(const Db::AppStateEntity& appStateEntity);

  // mutations
  void setFromEntityAndScene(const Db::AppStateEntity& appStateEntity, const Db::Scene& newScene) {
    id.store(appStateEntity.id);
    audioFramesPerBuffer.store(appStateEntity.audioFramesPerBuffer);
    playState.store( STOP);
    crossfade.store(appStateEntity.crossfade);
    scene.store(newScene);
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

  Db::Scene getScene() const {
    return scene.load();
  };

  void setScene(const Db::Scene& val) {
    scene.store(val);
  };

  Db::SceneID getSceneId() const {
    return scene.load().id;
  }

  Db::Version getSceneVersion() const {
    return scene.load().version;
  }

  sf_count_t getCrossfade() const {
    return crossfade.load();
  }
  void setCrossfade(const sf_count_t val) {
    crossfade.store(val);
  }

  std::string toString() const {
    return
      " Gj::AppState { "
      "  id: " + std::to_string(id) +
      "  audioFramesPerBuffer: " + std::to_string(audioFramesPerBuffer) +
      "  playState: " + std::to_string(playState) +
      "  sceneId: " + std::to_string(scene.load().id) +
      " } ";
  };

  Result setCurrentlyPlaying(const Db::DecoratedAudioFile& decoratedAudioFile) {
    currentlyPlaying.store(decoratedAudioFile);
    return OK;
  }

  Db::DecoratedAudioFile getCurrentlyPlaying() {
    return currentlyPlaying.load();
  }
};

} // Gj

#endif //APPSTATE_H
