//
// Created by ns on 10/6/25.
//

#ifndef TRACKDECORATE_H
#define TRACKDECORATE_H

#include <sndfile.h>
#include <sqlite3.h>

#include "../../Types.h"
#include "../../Logging.h"
#include "../../../enums/AudioCodec.h"
#include "../../../enums/Result.h"

#include "../../entity/musicLibrary/Album.h"
#include "../../entity/musicLibrary/Artist.h"
#include "../../entity/musicLibrary/AudioFile.h"
#include "../../entity/musicLibrary/Genre.h"
#include "../../entity/musicLibrary/Track.h"

namespace Gj {
namespace Db {

struct DecoratedAudioFile final {
  DecoratedAudioFile()
    : album(Album(" - "))
    , artist(Artist(" - "))
    , genre(" - ")
    {}


  DecoratedAudioFile(
    Album album,
    Artist artist,
    AudioFile audioFile,
    Genre genre,
    Track track
  );

  static DecoratedAudioFile deser(sqlite3_stmt* stmt);

  Album album;
  Artist artist;
  AudioFile audioFile;
  Genre genre;
  Track track;

  bool isValid() const {
    return audioFile.valid;
  }
};

} // Db
} // Gj

#endif //TRACKDECORATE_H
