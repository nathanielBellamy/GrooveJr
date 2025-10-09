//
// Created by ns on 10/6/25.
//

#include "AlbumRepository.h"

namespace Gj {
namespace Db {

AlbumRepository::AlbumRepository(sqlite3** db, AppState* gAppState)
  : db(db)
  , gAppState(gAppState)
  {}

ID AlbumRepository::save(const Album& album) const {
  const std::string query = R"sql(
    insert into albums (title, year)
    values (?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::AlbumRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_text(stmt, 1, album.title.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, album.year);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::AlbumRepository::save",
      "Failed to save Album " + album.title + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::AlbumRepository::save",
      "Saved Album: " + album.title
    );
  }

  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}

ID AlbumRepository::save(const AlbumWithArtist& albumWithArtist) const {
  ID albumMatch = getAlbumIdWithArtist(albumWithArtist.album, albumWithArtist.artist);
  if (albumMatch != 0) {
    Logging::write(
        Info,
        "Db::AlbumRepository::save",
        "Album " + albumWithArtist.album.title + " by Artist " + albumWithArtist.artist.name + " Already Exists"
    );
    return albumMatch;
  }

  const ID albumId = save(albumWithArtist.album);
  Album albumWithId (albumId, albumWithArtist.album.title, albumWithArtist.album.year);

  join(albumWithId, albumWithArtist.artist);

  return albumId;
}

Result AlbumRepository::join(const Album& album, const Artist& artist) const {
  const std::string joinQuery = R"sql(
    insert into artist_to_albums (artistId, albumId)
    values (?, ?);
  )sql";

  sqlite3_stmt* joinStmt;
  if (sqlite3_prepare_v2(*db, joinQuery.c_str(), -1, &joinStmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::AlbumRepository::join(album, artist)",
      "Failed to prepare join statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return ERROR;
  }

  sqlite3_bind_int(joinStmt, 1, artist.id);
  sqlite3_bind_int(joinStmt, 2, album.id);

  if (sqlite3_step(joinStmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::AlbumRepository::join(album, artist)",
      "Failed to join Album " + album.title + " to Artist " + artist.name + ". Message: " + std::string(sqlite3_errmsg(*db))
    );
    return ERROR;
  }

  Logging::write(
    Info,
    "Db::EffectRepository::save",
    "Joined Album " + album.title + " to Artist " + artist.name + ". Message: " + std::string(sqlite3_errmsg(*db))
  );

  return OK;
}

ID AlbumRepository::getAlbumIdWithArtist(const Album& album, const Artist& artist) const {
  const std::string query = R"sql(
    select id from albums
    right join artist_to_albums
    on artist_to_albums.albumId = albums.id
    where albums.title = ?
      and artist_to_albums.artistId = ?
    limit 1
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::AlbumRepository::getChannels",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_text(stmt, 1, album.title.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, artist.id);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const ID id = sqlite3_column_int(stmt, 0);
    return id;
  }

  return 0;
}

ID AlbumRepository::saveAll(const std::vector<Album>& album) const {

  // TODO

  return 0;
};

} // Db
} // Gj
