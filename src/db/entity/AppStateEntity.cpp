//
// Created by ns on 5/29/25.
//

#include "AppStateEntity.h"

namespace Gj {
namespace Db {

AppStateEntity::AppStateEntity(
  const ID id,
  const jack_nframes_t audioFramesPerBuffer,
  const ID sceneId,
  const sf_count_t crossfade,
  const ID currentlyPlaying)
  : id(id)
  , audioFramesPerBuffer(audioFramesPerBuffer)
  , sceneId(sceneId)
  , crossfade(crossfade)
  , currentlyPlaying(currentlyPlaying)
  {}

AppStateEntity AppStateEntity::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const int audioFramesPerBuffer = sqlite3_column_int(stmt, 1);
  const ID sceneId = sqlite3_column_int(stmt, 2);
  const sf_count_t crossfade = sqlite3_column_int(stmt, 3);
  const ID currentlyPlaying = sqlite3_column_int(stmt, 4);

  return {
    id,
    static_cast<jack_nframes_t>(audioFramesPerBuffer),
    sceneId,
    crossfade,
    currentlyPlaying
  };
}

AppStateEntity AppStateEntity::base() {
  return {
    0, 256, 0, 10000, 0
  };
}

} // Db
} // Gj