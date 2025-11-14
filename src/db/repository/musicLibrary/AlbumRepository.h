//
// Created by ns on 10/6/25.
//

#ifndef ALBUMREPOSITORY_H
#define ALBUMREPOSITORY_H

#include <vector>

#include "../../../enums/Result.h"
#include "../../../types/AtomicStr.h"

#include "MusicLibraryRepository.h"
#include "../../dto/musicLibrary/AlbumWithArtist.h"
#include "../../entity/musicLibrary/Album.h"
#include "../../Types.h"

namespace Gj {
namespace Db {

class AlbumRepository final : public MusicLibraryRepository {

  public:
    AlbumRepository(sqlite3** db, AppState* gAppState)
      : MusicLibraryRepository(db, gAppState)
      {};

    std::vector<Album> getAll() const;
    ID save(const Album& artist) const;
    ID save(const AlbumWithArtist& albumWithArtist) const;
    ID saveAll(const std::vector<Album>& artist) const;
    Result join(const Album& album, const Artist& artist) const;

    ID getAlbumIdWithArtist(const Album& album, const Artist& artist) const;

};

} // Db
} // Gj



#endif //ALBUMREPOSITORY_H
