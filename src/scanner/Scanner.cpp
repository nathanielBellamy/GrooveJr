//
// Created by ns on 10/4/25.
//

#include "Scanner.h"

namespace Gj {
namespace Scanner {
namespace fs = std::filesystem;

// TODO
Scanner::Scanner(Db::Dao* dao)
: dao(dao) {
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

  Db::Playlist playlist("Grooves With Moves");
  dao->playlistRepository.save(playlist);
  dao->playlistRepository.save(playlist);

  for (const auto& entry: fs::directory_iterator(dirPath)) {
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

      // Use to8Bit(true) to force UTF-8 encoding.
      // The default to8Bit() uses Latin-1, which produces
      // empty or garbled strings for non-ASCII metadata.
      std::string artistName = tag->artist().to8Bit(true);
      if (artistName.empty()) {
        artistName = "Unknown Artist";
        Logging::write(
          Warning,
          "Scanner::Scanner::runScan()",
          "Missing artist tag, using fallback for: " + entry.path().string()
        );
      }
      Db::Artist artist(artistName);
      artist.id = dao->artistRepository.save(artist);

      std::string albumTitle = tag->album().to8Bit(true);
      if (albumTitle.empty()) {
        albumTitle = "Unknown Album";
        Logging::write(
          Warning,
          "Scanner::Scanner::runScan()",
          "Missing album tag, using fallback for: " + entry.path().string()
        );
      }
      Db::Album album(albumTitle, tag->year());
      Db::AlbumWithArtist albumWithArtist{
        album,
        artist
      };
      album.id = dao->albumRepository.save(albumWithArtist);

      std::string trackTitle = tag->title().to8Bit(true);
      if (trackTitle.empty()) {
        trackTitle = entry.path().stem().string();
        Logging::write(
          Warning,
          "Scanner::Scanner::runScan()",
          "Missing title tag, using filename as fallback for: " + entry.path().string()
        );
      }
      Db::Track track(album.id, trackTitle, tag->track());
      track.id = dao->trackRepository.save(track);
      dao->trackRepository.join(track, artist);

      Db::AudioFile audioFile(track.id, entry.path().string().c_str());
      audioFile.id = dao->audioFileRepository.save(audioFile);

      std::string genreName = tag->genre().to8Bit(true);
      Db::Genre genre(genreName);
      Db::GenreWithTrackId genreWithTrackId{genre, track.id};
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
