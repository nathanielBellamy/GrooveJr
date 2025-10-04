//
// Created by ns on 5/29/25.
//

#include "AppStateEntity.h"

namespace Gj {
namespace Db {

AppStateEntity::AppStateEntity(
  const int id,
  const jack_nframes_t audioFramesPerBuffer,
  const int sceneId,
  const int sceneIndex,
  const sf_count_t crossfade)
  : id(id)
  , audioFramesPerBuffer(audioFramesPerBuffer)
  , sceneId(sceneId)
  , sceneIndex(sceneIndex)
  , crossfade(crossfade)
  {}

AppStateEntity AppStateEntity::deser(sqlite3_stmt* stmt) {
  const int id = sqlite3_column_int(stmt, 0);
  const int audioFramesPerBuffer = sqlite3_column_int(stmt, 1);
  const int sceneId = sqlite3_column_int(stmt, 2);
  const int sceneIndex = sqlite3_column_int(stmt, 3);
  const sf_count_t crossfade = sqlite3_column_int(stmt, 4);

  return {
    id,
    static_cast<jack_nframes_t>(audioFramesPerBuffer),
    sceneId,
    sceneIndex,
    crossfade
  };
}

AppStateEntity AppStateEntity::base() {
  return {
    0, 256, 0, 0, 100000
  };
}

} // Db
} // Gj