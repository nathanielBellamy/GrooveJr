//
// Created by ns on 10/6/25.
//

#ifndef GENREREPOSITORY_H
#define GENREREPOSITORY_H

#include <optional>

#include "MusicLibraryRepository.h"
#include "../../../enums/Result.h"

#include "../../entity/musicLibrary/Genre.h"
#include "../../dto/musicLibrary/GenreWithTrackId.h"

namespace Gj {
namespace Db {

class GenreRepository final : public MusicLibraryRepository{

  public:
    GenreRepository(sqlite3** db, AppState* gAppState)
      : MusicLibraryRepository(db, gAppState)
      {};

    std::vector<Genre> getAll() const;
    ID save(const Genre& genre) const;
    ID save(const GenreWithTrackId& genreWithTrackId) const;
    std::optional<Genre> findByName(const std::string& name) const;
    Result join(const ID genreId, const ID trackId) const;
};


} // Db
} // Gj

#endif //GENREREPOSITORY_H
