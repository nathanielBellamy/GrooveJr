//
// Created by ns on 10/6/25.
//

#include "AudioFileRepository.h"

namespace Gj {
namespace Db {

ID AudioFileRepository::save(const AudioFile& audioFile) const {
  const std::string query = R"sql(
    insert into audioFiles (trackId, filePath, valid, sf_frames, sf_sampleRate, sf_channels, sf_format, sf_sections, sf_seekable)
    values (?, ?, ?, ?, ?, ?, ?, ?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::AudioFileRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, audioFile.trackId);
  sqlite3_bind_text(stmt, 2, audioFile.filePath.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, audioFile.valid);
  sqlite3_bind_int(stmt, 4, audioFile.sfInfo.frames);
  sqlite3_bind_int(stmt, 5, audioFile.sfInfo.samplerate);
  sqlite3_bind_int(stmt, 6, audioFile.sfInfo.channels);
  sqlite3_bind_int(stmt, 7, audioFile.sfInfo.format);
  sqlite3_bind_int(stmt, 8, audioFile.sfInfo.sections);
  sqlite3_bind_int(stmt, 9, audioFile.sfInfo.seekable);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    const char* errmsg = sqlite3_errmsg(*db);

    if (std::strstr(errmsg, "UNIQUE constraint failed: audioFiles.filePath") != nullptr) {
      const std::optional<AudioFile> found = findByFilePath(audioFile.filePath);
      if (found.has_value()) {
        // TODO: update validity of existing file
        const AudioFile& foundAudioFile = found.value();
        Logging::write(
            Warning,
            "Db::AudioFileRepository::save",
            "Not Saving AudioFile " + audioFile.filePath + ". Already Exists - SQLite UNIQUE constraint failed: audioFiles.filePath"
        );
      }
      return found.value().id;
    }
    Logging::write(
      Error,
      "Db::AudioFileRepository::save",
      "Failed to save AudioFile " + audioFile.filePath + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::AudioFileRepository::save",
      "Saved AudioFile: " + audioFile.filePath
    );
  }

  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}

std::optional<AudioFile> AudioFileRepository::findByFilePath(const std::string& filePath) const {
  const std::string query = R"sql(
    select * from audioFiles a
    where a.filePath = ?;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::AudioFileRepository::findByFilePath",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return std::optional<AudioFile>();
  }

  sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) == SQLITE_ROW)
    return std::optional(AudioFile::deser(stmt));

  return std::optional<AudioFile>();
}


std::optional<DecoratedAudioFile> AudioFileRepository::findDecoratedAudioFileById(ID id) const {
  const std::string query = R"sql(
    select
      alb.id,
      alb.title,
      alb.year,
      art.id,
      art.name,
      af.id,
      af.filePath,
      af.valid,
      af.sf_frames,
      af.sf_sampleRate,
      af.sf_channels,
      af.sf_format,
      af.sf_sections,
      af.sf_seekable,
      g.id,
      g.name,
      trk.id,
      trk.title,
      trk.trackNumber
      from audioFiles af
    join tracks trk
    on af.trackId = trk.id
    join albums alb
    on trk.albumId = alb.id
    join track_to_artists tta
    on tta.trackId = trk.id
    join track_to_genres ttg
    on ttg.trackId = trk.id
    join genres g
    on g.id = ttg.genreId
    join artists art
    on tta.artistId = art.id
    where af.id = ?
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::AudioFileRepository::findByFilePath",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return std::optional<DecoratedAudioFile>();
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) == SQLITE_ROW)
    return std::optional(DecoratedAudioFile::deser(stmt));

  return std::optional<DecoratedAudioFile>();

}

} // Db
} // Gj
