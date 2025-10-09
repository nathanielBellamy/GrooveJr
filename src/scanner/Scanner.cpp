//
// Created by ns on 10/4/25.
//

#include "Scanner.h"

namespace Gj {
namespace Scanner {

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

Result Scanner::runScan() {
  Logging::write(
      Info,
      "Scanner::Scanner::runScan()",
      "Running Scanner"
  );

  const char* filePath1 = "/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/05 Back To Black.flac";
  const char* filePath2 = "/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/06 Love Is A Losing Game.flac";
  const char* filePath3 = "/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/07 Tears Dry On Their Own.flac";
  const char* filePath4 = "/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/07 Tears Dry On Their Own.flac";

  const char* filePaths [4] = { filePath1, filePath2, filePath3, filePath4 };

  for (const auto path : filePaths) {
    TagLib::FileRef file(
        path,
        true,
        TagLib::AudioProperties::Accurate
    );

    if (file.isNull() || !file.tag()) {
      Logging::write(
          Error,
          "Scanner::Scanner::runScan()",
          "Failed to read file."
      );
      return ERROR;
    }

    TagLib::Tag* tag = file.tag();

    std::cout << "Title       : " << tag->title() << std::endl;
    std::cout << "Artist      : " << tag->artist() << std::endl;
    std::cout << "Album       : " << tag->album() << std::endl;
    std::cout << "Year        : " << tag->year() << std::endl;
    std::cout << "Track Number: " << tag->track() << std::endl;
    std::cout << "Genre       : " << tag->genre() << std::endl;

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

    Db::AudioFile audioFile (track.id, path);
    audioFile.id = dao->audioFileRepository.save(audioFile);
  }


  Logging::write(
      Info,
      "Scanner::Scanner::runScan()",
      "Scanner Scanned Successfully"
  );

  return OK;
}

} // Scn
} // Gj