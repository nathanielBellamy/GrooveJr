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
#include "repository/musicLibrary/AlbumRepository.h"
#include "repository/musicLibrary/ArtistRepository.h"
#include "repository/musicLibrary/AudioFileRepository.h"
#include "repository/musicLibrary/CacheRepository.h"
#include "repository/musicLibrary/GenreRepository.h"
#include "repository/musicLibrary/PlaylistRepository.h"
#include "repository/musicLibrary/QueueRepository.h"
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
    AlbumRepository albumRepository;
    ArtistRepository artistRepository;
    AudioFileRepository audioFileRepository;
    CacheRepository cacheRepository;
    GenreRepository genreRepository;
    PlaylistRepository playlistRepository;
    QueueRepository queueRepository;
    TrackRepository trackRepository;

    Dao(AppState* gAppState);
    ~Dao();
    int initDb();
    int initSchema() const;
};

} // Db
} // Gj




#endif //DATABASE_H
