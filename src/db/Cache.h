//
// Created by ns on 10/11/25.
//

#ifndef CACHE_H
#define CACHE_H

#include <map>

#include "Dao.h"
#include "entity/musicLibrary/Album.h"
#include "entity/musicLibrary/Artist.h"
#include "entity/musicLibrary/AudioFile.h"
#include "entity/musicLibrary/Genre.h"
#include "entity/musicLibrary/Track.h"
#include "Types.h"

#include "../enums/Result.h"

namespace Gj {
namespace Db {

// NOTE:
  // - the Cache class is not itself thread safe
  // - should be instantiated on/owned by an actor
  // - actors provide thread safety
class Cache final {
  Dao* dao;
  std::map<ID, Album> albums;
  std::map<ID, Artist> artists;
  std::map<ID, AudioFile> audioFiles;
  std::map<ID, Genre> genres;
  std::map<ID, Track> tracks;

  public:
    Cache(Dao* dao)
      : dao(dao)
      {}

    Result loadTracksAll();
};

} // Db
} // Gj

#endif //CACHE_H
