//
// Created by ns on 5/29/25.
//

#ifndef APPSTATEENTITY_H
#define APPSTATEENTITY_H

#include <sqlite3.h>
#include <sndfile.h>
#include <jack/jack.h>

namespace Gj {
namespace Db {


struct AppStateEntity {
  int id;
  jack_nframes_t audioFramesPerBuffer;
  int sceneId;
  int sceneIndex;
  sf_count_t crossfade;

  AppStateEntity(int id, jack_nframes_t audioFramesPerBuffer, int sceneId, int sceneIndex, sf_count_t crossfade);

  static AppStateEntity deser(sqlite3_stmt* stmt);

  static AppStateEntity base();
};

} // Db
} // Gj

#endif //APPSTATE_H
