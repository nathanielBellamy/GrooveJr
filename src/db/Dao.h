//
// Created by ns on 5/25/25.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <filesystem>

#include <sqlite3.h>

#include "Logging.h"
#include "../AppState.h"

#include "repository/mixer/ChannelRepository.h"
#include "repository/mixer/EffectRepository.h"
#include "repository/mixer/SceneRepository.h"
#include "repository/musicLibrary/TrackRepository.h"
#include "repository/AppStateRepository.h"

namespace Gj {
namespace Db {

class Dao {
  sqlite3* db;
  AppState* gAppState;

  public:
    AppStateRepository appStateRepository;
    ChannelRepository channelRepository;
    EffectRepository effectRepository;
    SceneRepository sceneRepository;
    TrackRepository trackRepository;

    Dao(AppState* gAppState);
    ~Dao();
    int initDb();
    int initSchema() const;
};

} // Db
} // Gj




#endif //DATABASE_H
