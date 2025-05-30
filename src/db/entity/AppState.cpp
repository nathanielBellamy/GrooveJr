//
// Created by ns on 5/29/25.
//

#include "AppState.h"

namespace Gj {
namespace Db {

AppState::AppState(const int audioFramesPerBuffer, const int sceneId, const int sceneIndex)
  : audioFramesPerBuffer(audioFramesPerBuffer)
  , sceneId(sceneId)
  , sceneIndex(sceneIndex)
  {}

AppState AppState::deser(sqlite3_stmt* stmt) {
  const int audioFramesPerBuffer = sqlite3_column_int(stmt, 0);
  const int sceneId = sqlite3_column_int(stmt, 1);
  const int sceneIndex = sqlite3_column_int(stmt, 2);

  return AppState(audioFramesPerBuffer, sceneId, sceneIndex);
}

} // Db
} // Gj