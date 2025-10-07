//
// Created by ns on 10/6/25.
//

#include "TrackDecorate.h"

namespace Gj {
namespace Db {

TrackDecorate::TrackDecorate(
  const std::string& filePath,
  const std::string& title,
  const std::string& artist,
  const std::string& album,
  uint16_t trackNumber
)
  : title(title)
  , artist(artist)
  , album(album)
  , trackNumber(trackNumber)
  , track(0, title)
  , audioFile(filePath)
  {

  std::cout << "Instantiated TrackDecorate " << filePath << std::endl
    << "Title: " << title << std::endl
    << "Artist: " << artist << std::endl
    << "Album: " << album << std::endl
    << "Track number: " << trackNumber << std::endl;

  Logging::write(
      Info,
      "Db::TrackDecorate::TrackDecorate",
      "Created TrackDecorate for track title: " + title
  );
}

} // Db
} // Gj