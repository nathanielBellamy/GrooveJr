//
// Created by ns on 10/6/25.
//

#ifndef ALBUMREPOSITORY_H
#define ALBUMREPOSITORY_H

#include <vector>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../AppState.h"
#include "../../../enums/Result.h"

#include "../../dto/musicLibrary/AlbumWithArtist.h"
#include "../../entity/musicLibrary/Album.h"
#include "../../Types.h"

namespace Gj {
namespace Db {

class AlbumRepository final {

  sqlite3** db;
  AppState* gAppState;

  public:
    AlbumRepository(sqlite3** db, AppState* gAppState);
    std::vector<Album> getAll() const;
    ID save(const Album& artist) const;
    ID save(const AlbumWithArtist& albumWithArtists) const;
    ID saveAll(const std::vector<Album>& artist) const;
    Result join(const Album& album, const Artist& artist) const;

    ID getAlbumIdWithArtist(const Album& album, const Artist& artist) const;

};

} // Db
} // Gj



#endif //ALBUMREPOSITORY_H
