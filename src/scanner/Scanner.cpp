//
// Created by ns on 10/4/25.
//

#include "Scanner.h"

namespace Gj {
namespace Scanner {

  namespace fs = std::filesystem;

// TODO
Scanner::Scanner(Db::Dao* dao)
  : dao(dao)
  {

  Logging::write(
      Info,
      "Scanner::Scanner::Scanner()",
      "Instantiated Scanner"
  );
}

Result Scanner::scanDirectoryRecursive(const std::string& dirPath) const {
  Logging::write(
      Info,
      "Scanner::Scanner::runScan()",
      "Scanning dirPath: " + dirPath
  );

  if (!fs::exists(dirPath)) {
    Logging::write(
        Info,
        "Scanner::Scanner::runScan()",
        "Directory does not exist: " + dirPath
    );
    return ERROR;
  }

  Db::Playlist playlist ("Grooves With Moves");
  dao->playlistRepository.save(playlist);
  dao->playlistRepository.save(playlist);

  for (const auto& entry : fs::directory_iterator(dirPath)) {
    if (fs::is_directory(entry.status())) {
      // recurse into subdirectory
      scanDirectoryRecursive(entry.path());
    } else if (fs::is_regular_file(entry.status())) {
      const std::string ext = entry.path().extension().string();

      // TODO: m4a support
      const bool isAudioFile = ext == ".wav"
                               || ext == ".flac"
                               || ext == ".mp3"
                               || ext == ".mp4"
                               || ext == ".aif"
                               || ext == ".ogg"
                               || ext == ".alac";

      if (!isAudioFile)
        continue;

      TagLib::FileRef file(
          entry.path().string().c_str(),
          true,
          TagLib::AudioProperties::Accurate
      );

      if (file.isNull() || !file.tag()) {
        Logging::write(
            Error,
            "Scanner::Scanner::runScan()",
            "Failed to read file."
        );
        continue;
      }

      TagLib::Tag* tag = file.tag();

      Db::Artist artist (tag->artist().to8Bit());
      artist.id = dao->artistRepository.save(artist);

      Db::Album album (tag->album().to8Bit(), tag->year());
      Db::AlbumWithArtist albumWithArtist {
        album,
        artist
      };
      album.id = dao->albumRepository.save(albumWithArtist);

      Db::Track track (album.id, tag->title().to8Bit(), tag->track());
      track.id = dao->trackRepository.save(track);

      Db::AudioFile audioFile (track.id, entry.path().string().c_str());
      audioFile.id = dao->audioFileRepository.save(audioFile);

      Db::Genre genre (tag->genre().to8Bit());
      Db::GenreWithTrackId genreWithTrackId { genre, track.id };
      dao->genreRepository.save(genreWithTrackId);
    }
  }

  Logging::write(
      Info,
      "Scanner::Scanner::runScan()",
      "Done Scanning dirPath: " + dirPath
  );

  return OK;
}

} // Scn
} // Gj