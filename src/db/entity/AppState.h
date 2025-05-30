//
// Created by ns on 5/29/25.
//

#ifndef APPSTATE_H
#define APPSTATE_H

#include <sqlite3.h>

namespace Gj {
namespace Db {

// todo: rename to avoid collision or forgo this struct
struct AppState {
  int audioFramesPerBuffer;
  int sceneId;
  int sceneIndex;

  AppState(int audioFramesPerBuffer, int sceneId, int sceneIndex);

  static AppState deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj

#endif //APPSTATE_H
