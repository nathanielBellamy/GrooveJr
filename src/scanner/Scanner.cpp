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

  const char* filePaths [3] = { filePath1, filePath2, filePath3 };

  for (int i = 0; i < 3; ++i) {
    // (FileName fileName, bool readAudioProperties=true, AudioProperties::ReadStyle audioPropertiesStyle=AudioProperties::Average)
    TagLib::FileRef file(
        filePaths[i],
        true,
        TagLib::AudioProperties::Average
    );

    if (file.isNull() || !file.tag()) {
      std::cerr << "Failed to read file: " << filePaths[i] << std::endl;
      return ERROR;
    }

    TagLib::Tag* tag = file.tag();

    std::cout << "Title       : " << tag->title() << std::endl;
    std::cout << "Artist      : " << tag->artist() << std::endl;
    std::cout << "Album       : " << tag->album() << std::endl;
    std::cout << "Year        : " << tag->year() << std::endl;
    std::cout << "Track Number: " << tag->track() << std::endl;
    std::cout << "Genre       : " << tag->genre() << std::endl;

    Db::Track track(
        filePaths[i],
        tag->title().to8Bit(),
        tag->artist().to8Bit(),
        tag->album().to8Bit(),
        static_cast<uint16_t>(tag->track())
    );
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