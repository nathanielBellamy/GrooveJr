//
// Created by ns on 10/6/25.
//

#include "DecoratedAudioFile.h"

namespace Gj {
namespace Db {

DecoratedAudioFile::DecoratedAudioFile(Album album, Artist artist, AudioFile audioFile, Genre genre, Track track)
  : album(album)
  , artist(artist)
  , audioFile(audioFile)
  , genre(genre)
  , track(track) {
  Logging::write(
    Info,
    "Db::TrackDecorate::TrackDecoratedDto",
    "Created TrackDecorate for track title: " + track.title.std_str()
  );
}

DecoratedAudioFile DecoratedAudioFile::deser(sqlite3_stmt *stmt) {
  const ID albumId = sqlite3_column_int(stmt, 0);
  const unsigned char *albumTitle = sqlite3_column_text(stmt, 1);
  const Year albumYear = sqlite3_column_int(stmt, 2);
  const ID artistId = sqlite3_column_int(stmt, 3);
  const unsigned char *artistName = sqlite3_column_text(stmt, 4);
  const ID audioFileId = sqlite3_column_int(stmt, 5);
  const unsigned char *audioFilePath = sqlite3_column_text(stmt, 6);
  const bool audioFileValid = sqlite3_column_int(stmt, 7);
  const sf_count_t sf_frames = sqlite3_column_int(stmt, 8);
  const int sf_sampleRate = sqlite3_column_int(stmt, 9);
  const int sf_channels = sqlite3_column_int(stmt, 10);
  const int sf_format = sqlite3_column_int(stmt, 11);
  const int sf_sections = sqlite3_column_int(stmt, 12);
  const int sf_seekable = sqlite3_column_int(stmt, 13);
  const ID genreId = sqlite3_column_int(stmt, 14);
  const unsigned char *genreName = sqlite3_column_text(stmt, 15);
  const ID trackId = sqlite3_column_int(stmt, 16);
  const unsigned char *trackTitle = sqlite3_column_text(stmt, 17);
  const TrackNumber trackNumber = sqlite3_column_int(stmt, 18);

  Album album(
    albumId,
    std::string(reinterpret_cast<const char *>(albumTitle)),
    albumYear
  );

  Artist artist(
    artistId,
    std::string(reinterpret_cast<const char *>(artistName))
  );

  SF_INFO sfInfo{
    sf_frames,
    sf_sampleRate,
    sf_channels,
    sf_format,
    sf_sections,
    sf_seekable
  };

  AudioFile audioFile(
    audioFileId,
    trackId,
    std::string(reinterpret_cast<const char *>(audioFilePath)),
    audioFileValid,
    sfInfo
  );

  Genre genre(
    genreId,
    std::string(reinterpret_cast<const char *>(genreName))
  );

  Track track(
    trackId,
    albumId,
    std::string(reinterpret_cast<const char *>(trackTitle)),
    trackNumber
  );

  return DecoratedAudioFile(
    album,
    artist,
    audioFile,
    genre,
    track
  );
}
} // Db
} // Gj
